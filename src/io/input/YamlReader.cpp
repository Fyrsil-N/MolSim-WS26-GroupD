//
// Created by fmathis on 13/11/2026.
//

#include "YamlReader.h"

#include <math.h>
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <string>
#include <utility>

#include "ParticleContainer/LinkedCellContainer.h"
#include "ParticleContainer/ParticleContainer.h"
#include "SimulationParameters.h"
#include "forces/Gravity.h"
#include "utils/ArrayUtils.h"
#include "utils/MaxwellBoltzmannDistribution.h"

namespace io {
YamlReader::YamlReader(std::string filename) : filename(std::move(filename)) {
    try {
        file = YAML::LoadFile(this->filename);
    } catch (const YAML::Exception &e) {
        spdlog::error("'{}': {}", this->filename, e.what());
        std::exit(EXIT_FAILURE);
    }

    if (!file.IsMap()) {
        spdlog::error("'{}': unexpected root type, should be Map", this->filename);
        std::exit(EXIT_FAILURE);
    }
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

ShapeParameters YamlReader::parseShapeParams(YAML::Node root, const bool third_dim) {
    ShapeParameters params;

    if (root["anchor_point"]) {
        params.anchor_point = root["anchor_point"].as<std::array<double, 3>>(std::array<double, 3>{0, 0, 0});
    }

    if (root["dimensions"]) {
        params.particle_dims = root["dimensions"].as<std::array<size_t, 3>>(std::array<size_t, 3>{1, 1, 1});
    }

    if (root["velocity"]) {
        params.velocity = root["velocity"].as<std::array<double, 3>>(std::array<double, 3>{0, 0, 0});
    }

    if (root["spacing"]) {
        params.spacing = root["spacing"].as<double>(1);
    }

    if (root["mass"]) {
        params.particle_mass = root["mass"].as<double>(1);
    }

    if (root["type"]) {
        params.type = root["type"].as<int>(0);
    }

    if (!third_dim) {
        params.anchor_point.at(2) = 1;
        params.particle_dims.at(2) = 1;
        params.velocity.at(2) = 0;
    }

    return params;
}

void YamlReader::parsePairwiseForce(YAML::Node root, SimulationParameters &params) {
    if (root.IsScalar()) {
        if (root.as<std::string>() == "Lennard-Jones") {
            params.pair_force = std::make_unique<forces::LennardJones>();
            return;
        }

        if (root.as<std::string>() == "Gravity") {
            params.pair_force = std::make_unique<forces::Gravity>();
            return;
        }
    } else if (root["Lennard-Jones"] && root["Lennard-Jones"].IsMap()) {
        auto lj_params = root["Lennard-Jones"];
        double sigma = 1;
        double epsilon = 5;
        double cutoff = 3;

        if (lj_params["sigma"]) {
            sigma = lj_params["sigma"].as<double>(1);
        }

        if (lj_params["epsilon"]) {
            epsilon = lj_params["epsilon"].as<double>(5);
        }

        if (lj_params["cutoff"]) {
            cutoff = lj_params["cutoff"].as<double>(3);
            params.particle_container = std::make_unique<LinkedCellContainer>(
                LinkedCellContainer(params.domain_anchor, params.domain_dimensions, params.third_dim, cutoff));
        }

        params.pair_force = std::make_unique<forces::LennardJones>(sigma, epsilon, cutoff);
        return;
    }

    spdlog::error("YamlReader Error: couldn't parse pairwise force information.");
    exit(EXIT_FAILURE);
}

std::array<bool, 2> YamlReader::parseDomainBehavior(const std::array<std::string, 2> &str_bounds) {
    std::array<bool, 2> result{};
    for (int i = 0; i < 2; i++) {
        if (str_bounds.at(i) == "reflecting") {
            result.at(i) = true;
            continue;
        }

        if (str_bounds.at(i) == "outflow") {
            result.at(i) = false;
            continue;
        }

        spdlog::error("'{}': Couldn't parse domain boundaries.", filename);
        exit(EXIT_FAILURE);
    }

    return result;
}

void YamlReader::parseDomainParams(YAML::Node root, SimulationParameters &params) {
    if (root["anchor_point"]) {
        params.domain_anchor = root["anchor_point"].as<std::array<double, 3>>(std::array<double, 3>{0, 0, 0});
    }

    if (root["dimensions"]) {
        params.domain_dimensions = root["dimensions"].as<std::array<double, 3>>(std::array<double, 3>{100, 100, 100});
    }

    if (root["boundaries"]) {
        if (!root.IsMap()) {
            spdlog::error("'{}': boundaries information should be map.", filename);
            exit(EXIT_FAILURE);
        }

        for (auto iter = root["boundaries"].begin(); iter != root["boundaries"].end(); ++iter) {
            if (!iter->first.IsScalar()) {
                spdlog::error("'{}': Couldn't parse domain boundaries.", filename);
                exit(EXIT_FAILURE);
            }

            auto cur = iter->first.as<std::string>();
            auto bounds = parseDomainBehavior(iter->second.as<std::array<std::string, 2>>());

            if (cur == "x") {
                for (int i = 0; i < 2; i++) {
                    params.boundary_conditions.at(i) = bounds.at(i);
                }
                continue;
            }

            if (cur == "y") {
                for (int i = 0; i < 2; i++) {
                    params.boundary_conditions.at(i + 2) = bounds.at(i);
                }
                continue;
            }

            if (params.third_dim && cur == "z") {
                for (int i = 0; i < 2; i++) {
                    params.boundary_conditions.at(i + 4) = bounds.at(i);
                }
                continue;
            }

            spdlog::error("'{}': Couldn't parse domain boundaries.", filename);
            exit(EXIT_FAILURE);
        }
    }

    if (!params.third_dim) {
        params.domain_anchor.at(2) = 0;
        params.domain_dimensions.at(2) = 1;
    }
}

void YamlReader::readInSimulation(SimulationParameters &params) {
    if (!file["simulation"]) {
        spdlog::info("'{}': no simulation information, using default values", filename);
        return;
    }

    auto sim_data = file["simulation"];
    if (!sim_data.IsMap()) {
        spdlog::error("'{}': simulation information should be map.", filename);
        exit(EXIT_FAILURE);
    }

    for (auto iter = sim_data.begin(); iter != sim_data.end(); ++iter) {
        auto cur = iter->first.as<std::string>();
        auto child = iter->second;

        if (cur == "delta_t") {
            params.delta_t = child.as<double>(0.014);
            continue;
        }

        if (cur == "end_t") {
            params.t_end = child.as<double>(1000);
            continue;
        }

        if (cur == "out_interval") {
            params.output_interval = child.as<int>(10);
            continue;
        }

        if (cur == "3d") {
            params.third_dim = child.as<bool>(false);
            continue;
        }

        if (cur == "domain") {
            parseDomainParams(child, params);
            continue;
        }

        if (cur == "pair_force") {
            parsePairwiseForce(child, params);
            continue;
        }

        spdlog::error("'{}': couldn't parse simulation information.", filename);
        exit(EXIT_FAILURE);
    }

    if (params.delta_t > params.t_end) {
        spdlog::warn("delta_t ({}) is larger than t_end ({})", params.delta_t, params.t_end);
    }
}

void YamlReader::readInParticles(ParticleContainer &particles, const bool third_dim) {
    if (!file["scene"]) {
        spdlog::info("'{}': no scene information", filename);
        return;
    }

    auto part_data = file["scene"];
    if (!part_data.IsMap()) {
        spdlog::error("'{}': scene information should be map.", filename);
        exit(EXIT_FAILURE);
    }

    for (auto iter = part_data.begin(); iter != part_data.end(); ++iter) {
        auto cur = iter->first.as<std::string>();
        auto child = iter->second.as<YAML::Node>(YAML::Node(YAML::NodeType::Undefined));
        if (!child.IsSequence()) {
            break;
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

        spdlog::error("'{}': couldn't parse scene information.", filename);
        exit(EXIT_FAILURE);
    }
}

void YamlReader::readInput(SimulationParameters &params) {
    readInSimulation(params);
    readInParticles(*params.particle_container, params.third_dim);
}

}  // namespace io