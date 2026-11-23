//
// Created by fmathis on 13/11/2026.
//

#include "YamlReader.h"

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <string>
#include <utility>

#include "InputError.h"
#include "ParticleContainer/LinkedCellContainer.h"
#include "SimulationParameters.h"
#include "utils/ArrayUtils.h"
#include "utils/MaxwellBoltzmannDistribution.h"

namespace io {
YamlReader::YamlReader(std::string filename) : filename(std::move(filename)) {
    try {
        root_node = YAML::LoadFile(this->filename);
    } catch (const YAML::Exception &e) {
        throw InputError(this->filename, e.mark.line + 1, e.msg);
    }

    if (!root_node.IsMap()) {
        fail(root_node, "unexpected root type, should be a map");
    }
}

void YamlReader::fail(const YAML::Node &node, const std::string &message) const {
    throw InputError(filename, node.Mark().line + 1, message);
}

void YamlReader::readInSimulation(SimulationParameters &params) {
    if (!root_node["simulation"]) {
        fail(root_node, "no simulation information provided. Info about the required format in the README.");
    }

    // catch yaml-cpp internal exceptions
    try {
        auto sim_data = root_node["simulation"];
        if (!sim_data.IsMap()) {
            fail(sim_data, "simulation information should be a map");
        }

        if (sim_data["basename"]) {
            params.output_file_basename = sim_data["basename"].as<std::string>();
        }

        if (sim_data["delta_t"] && sim_data["delta_t"].IsScalar()) {
            params.delta_t = sim_data["delta_t"].as<double>();
        } else {
            fail(sim_data, "delta_t is a required parameter");
        }

        if (sim_data["end_t"] && sim_data["end_t"].IsScalar()) {
            params.t_end = sim_data["end_t"].as<double>();
        } else {
            fail(sim_data, "end_t is a required parameter");
        }

        if (sim_data["out_interval"] && sim_data["out_interval"].IsScalar()) {
            params.output_interval = sim_data["out_interval"].as<int>();
        }

        if (sim_data["3d"] && sim_data["3d"].IsScalar()) {
            params.third_dim = sim_data["3d"].as<bool>();
        } else {
            spdlog::warn("No 3d parameter found defaulting to third_dim = false");
        }

        if (sim_data["domain"]) {
            parseDomainParams(sim_data["domain"], params);
        } else {
            fail(sim_data, "domain is a required parameter");
        }

        if (sim_data["pair_force"]) {
            parsePairwiseForce(sim_data["pair_force"], params);
        } else {
            fail(sim_data, "pair_force is a required parameter");
        }
    } catch (const YAML::Exception &e) {
        throw InputError(filename, e.mark.line + 1, e.msg);
    }

    if (params.delta_t > params.t_end) {
        spdlog::warn("delta_t ({}) is larger than t_end ({})", params.delta_t, params.t_end);
    }
}

void YamlReader::parsePairwiseForce(YAML::Node root, SimulationParameters &params) const {
    if (root.IsScalar() && root.as<std::string>() == "Gravity") {
        params.pair_force = GRAVITY;
    } else if (root["Lennard-Jones"] && root["Lennard-Jones"].IsMap()) {
        auto lj_params = root["Lennard-Jones"];
        params.pair_force = LENNARD_JONES;

        if (lj_params["sigma"]) {
            auto sigma = lj_params["sigma"].as<double>();
            if (sigma <= 0) {
                fail(root, "sigma has to be greater than zero");
            }
            params.sigma = sigma;
        } else {
            fail(root, "no sigma value set for lennard jones force");
        }

        if (lj_params["epsilon"]) {
            auto epsilon = lj_params["epsilon"].as<double>();
            if (epsilon <= 0) {
                fail(root, "epsilon has to be greater than zero");
            }
            params.epsilon = epsilon;
        } else {
            fail(root, "no epsilon value for lennard jones force");
        }

        if (lj_params["cutoff"]) {
            auto cutoff = lj_params["cutoff"].as<double>();
            if (cutoff <= 0) {
                spdlog::debug("cutoff radius is disabled (<=0)");
            }
            params.r_cutoff = cutoff;
        }
    } else {
        fail(root, "couldn't parse pair_force (expected Lennard-Jones or Gravity)");
    }
}

std::array<BoundaryCondition, 2> YamlReader::parseBoundaryCondition(const YAML::Node &node) const {
    const auto str_bounds = node.as<std::array<std::string, 2>>();

    std::array<BoundaryCondition, 2> result{};

    for (int i = 0; i < 2; i++) {
        if (str_bounds.at(i) == "reflecting") {
            result.at(i) = REFLECTING;
        } else if (str_bounds.at(i) == "outflow") {
            result.at(i) = OUTFLOW;
        } else {
            fail(node, "unknown boundary condition '" + str_bounds.at(i) + "' (expected reflecting or outflow)");
        }
    }

    return result;
}

void YamlReader::parseDomainParams(YAML::Node root, SimulationParameters &params) {
    if (!root.IsMap()) {
        fail(root, "domain information should be a map");
    }

    if (root["anchor_point"]) {
        params.domain_anchor = root["anchor_point"].as<std::array<double, 3>>();
    } else {
        fail(root, "missing anchor point for the domain boundary");
    }

    if (root["dimensions"]) {
        params.domain_dimensions = root["dimensions"].as<std::array<double, 3>>();
    } else {
        fail(root, "missing dimensions for the domain boundary");
    }

    if (root["boundaries"] && root["boundaries"].IsMap()) {
        auto boundaries = root["boundaries"];

        for (auto iter = boundaries.begin(); iter != boundaries.end(); ++iter) {
            if (!iter->first.IsScalar()) {
                fail(iter->first, "boundary axis should be x, y or z");
            }

            auto cur = iter->first.as<std::string>();
            auto bounds = parseBoundaryCondition(iter->second);

            auto index = 0;
            if (cur == "x") {
                index = 0;
            } else if (cur == "y") {
                index = 2;
            } else if (cur == "z") {
                index = 4;
            } else {
                fail(iter->first, "unknown boundary axis '" + cur + "'");
            }
            params.boundary_conditions[index] = bounds[0];
            params.boundary_conditions[index + 1] = bounds[1];
        }
    } else {
        fail(root, "malformed boundary conditions");
    }

    if (!params.third_dim) {
        params.domain_anchor.at(2) = 0;
        params.domain_dimensions.at(2) = 1;
    }
}

void YamlReader::readInParticles(ParticleContainer &particles, const bool third_dim) {
    if (!root_node["scene"]) {
        fail(root_node, "no scene information found");
    }

    // catch yaml-cpp internal exceptions
    try {
        auto scene_data = root_node["scene"];
        if (!scene_data.IsMap()) {
            fail(scene_data, "scene information should be a map");
        }

        for (auto iter = scene_data.begin(); iter != scene_data.end(); ++iter) {
            auto cur = iter->first.as<std::string>();
            auto child = iter->second.as<YAML::Node>(YAML::Node(YAML::NodeType::Undefined));
            if (!child.IsSequence()) {
                fail(iter->first, "'" + cur + "' should be a list of shapes");
            }

            if (cur == "cuboids") {
                generateShapes(generateCuboid, child, particles, third_dim);
                continue;
            }

            if (cur == "particles") {
                generateShapes(generateParticle, child, particles, third_dim);
                continue;
            }

            if (cur == "ellipsoids") {
                generateShapes(generateEllipsoid, child, particles, third_dim);
                continue;
            }

            fail(iter->first, "unknown scene key '" + cur + "' (expected cuboids, particles or ellipsoids)");
        }
    } catch (const YAML::Exception &e) {
        throw InputError(filename, e.mark.line + 1, e.msg);
    }
}

ShapeParameters YamlReader::parseShapeParams(YAML::Node root, const bool third_dim) {
    ShapeParameters shape;

    if (root["anchor_point"]) {
        shape.anchor_point = root["anchor_point"].as<std::array<double, 3>>();
    } else {
        fail(root, "anchor point is a required parameter");
    }

    if (root["dimensions"]) {
        shape.particle_dims = root["dimensions"].as<std::array<size_t, 3>>();
    }

    if (root["velocity"]) {
        shape.velocity = root["velocity"].as<std::array<double, 3>>();
    } else {
        fail(root, "velocity is a required parameter");
    }

    if (root["spacing"]) {
        shape.spacing = root["spacing"].as<double>();
    }

    if (root["mass"]) {
        shape.particle_mass = root["mass"].as<double>();
    } else {
        fail(root, "mass is a required parameter");
    }

    if (root["type"]) {
        shape.type = root["type"].as<int>();
    }

    if (!third_dim) {
        shape.anchor_point.at(2) = 1;
        shape.particle_dims.at(2) = 1;
        shape.velocity.at(2) = 0;
    }

    return shape;
}

void YamlReader::generateCuboid(ShapeParameters &params, ParticleContainer &particles, const bool third_dim) {
    particles.reserve(params.particle_dims.at(0) * params.particle_dims.at(1) * params.particle_dims.at(2));
    for (size_t x = 0; x < params.particle_dims.at(0); ++x) {
        for (size_t y = 0; y < params.particle_dims.at(1); ++y) {
            for (size_t z = 0; z < params.particle_dims.at(2); ++z) {
                std::array pos = {params.anchor_point.at(0) + static_cast<double>(x) * params.spacing,
                                  params.anchor_point.at(1) + static_cast<double>(y) * params.spacing,
                                  params.anchor_point.at(2) + static_cast<double>(z) * params.spacing};

                particles.emplaceBack(pos,
                                      params.velocity + maxwellBoltzmannDistributedVelocity(0.1, third_dim ? 3 : 2),
                                      params.particle_mass, params.type);
            }
        }
    }
    spdlog::debug("Cuboid created at ({}, {}, {}) with dimensions {}x{}x{}, h={}, m={}, type={}",
                  params.anchor_point.at(0), params.anchor_point.at(1), params.anchor_point.at(2),
                  params.particle_dims.at(0), params.particle_dims.at(1), params.particle_dims.at(2), params.spacing,
                  params.particle_mass, params.type);
}

void YamlReader::generateParticle(const ShapeParameters &params, ParticleContainer &particles, const bool third_dim) {
    particles.emplaceBack(params.anchor_point,
                          params.velocity + maxwellBoltzmannDistributedVelocity(0.1, third_dim ? 3 : 2),
                          params.particle_mass, params.type);
}

void YamlReader::generateEllipsoid(ShapeParameters &params, ParticleContainer &particles, const bool third_dim) {
    auto a = static_cast<long long>(params.particle_dims.at(0));
    auto b = static_cast<long long>(params.particle_dims.at(1));
    auto c = static_cast<long long>(params.particle_dims.at(2));

    particles.reserve(8 * a * b * c);

    for (long long x = 0; x < a; ++x) {
        for (long long y = 0; y < b; ++y) {
            for (long long z = 0; z < c; ++z) {
                if (static_cast<double>(x * x) / static_cast<double>(a * a) +
                        static_cast<double>(y * y) / static_cast<double>(b * b) +
                        static_cast<double>(z * z) / static_cast<double>(c * c) >
                    1) {
                    continue;
                }

                for (int i = 0; i < (third_dim ? 8 : 4); ++i) {
                    const int x_sign = i % 2 < 1 ? -1 : 1;
                    const int y_sign = i % 4 < 2 ? -1 : 1;
                    const int z_sign = i % 8 < 4 ? -1 : 1;

                    std::array<double, 3> pos{};
                    pos.at(0) = params.anchor_point.at(0) + x_sign * (static_cast<double>(x) + 0.5) * params.spacing;
                    pos.at(1) = params.anchor_point.at(1) + y_sign * (static_cast<double>(y) + 0.5) * params.spacing;
                    pos.at(2) =
                        third_dim ? params.anchor_point.at(2) + z_sign * (static_cast<double>(z) + 0.5) * params.spacing
                                  : 0;

                    particles.emplaceBack(pos,
                                          params.velocity + maxwellBoltzmannDistributedVelocity(0.1, third_dim ? 3 : 2),
                                          params.particle_mass, params.type);
                }
            }
        }
    }

    particles.shrinkToFit();

    spdlog::debug("Ellipsoid generated at ({}, {}, {}) with dimensions {}x{}x{}, h={}, m={}, type={}",
                  params.anchor_point.at(0), params.anchor_point.at(1), params.anchor_point.at(2), a, b, c,
                  params.spacing, params.particle_mass, params.type);
}
}  // namespace io