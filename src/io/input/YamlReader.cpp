//
// Created by fmathis on 13/11/2026.
//

#include "YamlReader.h"

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <string>
#include <utility>

#include "ParticleContainer.h"
#include "SimulationParameters.h"
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

void YamlReader::generateCuboid(const YAML::Node &root, ParticleContainer &particles) {
    ShapeParameters params = parseParameters(root);

    particles.reserve(params.particle_dims.at(0) * params.particle_dims.at(1) * params.particle_dims.at(2));
    for (unsigned int x = 0; x < params.particle_dims.at(0); ++x) {
        for (unsigned int y = 0; y < params.particle_dims.at(1); ++y) {
            for (unsigned int z = 0; z < params.particle_dims.at(2); ++z) {
                std::array pos = {params.anchor_point.at(0) + static_cast<double>(x) * params.spacing,
                                  params.anchor_point.at(1) + static_cast<double>(y) * params.spacing,
                                  params.anchor_point.at(2) + static_cast<double>(z) * params.spacing};
                particles.emplaceBack(pos, params.velocity + maxwellBoltzmannDistributedVelocity(0.1, 2),
                                      params.particle_mass, params.type);
            }
        }
    }
    spdlog::debug("Cuboid created at ({}, {}, {}) with dimensions {}x{}x{}, h={}, m={}, type={}",
                  params.anchor_point.at(0), params.anchor_point.at(1), params.anchor_point.at(2),
                  params.particle_dims.at(0), params.particle_dims.at(1), params.particle_dims.at(2), params.spacing,
                  params.particle_mass, params.type);
}

void YamlReader::generateParticle(const YAML::Node &root, ParticleContainer &particles) {
    ShapeParameters params = parseParameters(root);

    particles.reserve(1);
    particles.emplaceBack(params.anchor_point, params.velocity + maxwellBoltzmannDistributedVelocity(0.1, 2),
                          params.particle_mass, params.type);
}

ShapeParameters YamlReader::parseParameters(YAML::Node root) {
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

    return params;
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

        if (cur == "delta_t") {
            params.delta_t = iter->second.as<double>(0.014);
            continue;
        }

        if (cur == "end_t") {
            params.t_end = iter->second.as<double>(1000);
            continue;
        }

        if (cur == "out_interval") {
            params.output_interval = iter->second.as<int>(10);
            continue;
        }

        spdlog::error("'{}': couldn't parse simulation information.", filename);
        exit(EXIT_FAILURE);
    }
}

void YamlReader::readInParticles(ParticleContainer &particles) {
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
            generateShapes(generateCuboid, child, particles);
            continue;
        }

        if (cur == "particles") {
            generateShapes(generateParticle, child, particles);
            continue;
        }

        spdlog::error("'{}': couldn't parse scene information.", filename);
        exit(EXIT_FAILURE);
    }
}

}  // namespace io