//
// Created by luka-maric on 31.10.26.
//

#include "io/input/ParticleCreator.h"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <utility>

#include "ParticleContainer.h"
#include "utils/ArrayUtils.h"
#include "utils/MaxwellBoltzmannDistribution.h"
#include "utils/Parsers.h"

namespace io {

ParticleCreator::ParticleCreator(std::string filename) : config_filename(std::move(filename)) {}

void ParticleCreator::readInParticles(ParticleContainer &particles) {
    std::ifstream input_file(config_filename);

    if (!input_file.is_open()) {
        spdlog::error("could not open file '{}'", config_filename);
        std::exit(EXIT_FAILURE);
    }
    std::string tmp_string;
    while (input_file.good() && !input_file.eof()) {
        getline(input_file, tmp_string);
        tmp_string = std::string(trim(tmp_string));

        if (tmp_string.empty() || tmp_string.at(0) == '#') continue;

        if (tmp_string == "cuboid:") {
            parseCuboid(input_file, particles);
            continue;
        }

        if (tmp_string == "ellipsoid:") {
            parseEllipsoid(input_file, particles);
            continue;
        }

        spdlog::error("'{}': unknown form '{}'", config_filename, tmp_string);
        std::exit(EXIT_FAILURE);
    }
    if (particles.size() == 0) {
        spdlog::error("No particles were found.");
        std::exit(EXIT_FAILURE);
    }
}

void ParticleCreator::parseCuboid(std::ifstream &input_file, ParticleContainer &particles) {
    ShapeParameters params = parseParameters(input_file);

    particles.reserve(params.particle_dims.at(0) * params.particle_dims.at(1) * params.particle_dims.at(2));
    for (unsigned int x = 0; x < params.particle_dims.at(0); ++x) {
        for (unsigned int y = 0; y < params.particle_dims.at(1); ++y) {
            for (unsigned int z = 0; z < params.particle_dims.at(2); ++z) {
                std::array<double, 3> pos = {params.anchor_point.at(0) + static_cast<double>(x) * params.spacing,
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

void ParticleCreator::parseEllipsoid(std::ifstream &input_file, ParticleContainer &particles) {
    ShapeParameters params = parseParameters(input_file);

    (void)params;
    (void)particles;
    // NOT YET IMPLEMENTED BUT FUN

    spdlog::error("ellipsoids are not implemented yet.");
    std::exit(EXIT_FAILURE);
    ;
}

ShapeParameters ParticleCreator::parseParameters(std::ifstream &input_file) {
    ShapeParameters params;

    std::string tmp_string;
    std::array<double, 3> particle_count_in{};

    for (int i = 0; i < 6; ++i) {
        if (!input_file.good() || input_file.eof()) {
            if (i < 3) {
                spdlog::error("eof reached unexpectedly reading shape parameter {}", i + 1);
                std::exit(EXIT_FAILURE);
                ;
            }
            break;
        }

        // Retrieve next line
        getline(input_file, tmp_string);
        tmp_string = std::string(trim(tmp_string));

        if (tmp_string.empty()) {
            if (i < 3) {
                spdlog::error("End of definition reached unexpectedly reading shape parameter {}", i + 1);
                std::exit(EXIT_FAILURE);
                ;
            }
            break;
        }

        // ignore comments and empty lines
        if (tmp_string.at(0) == '#') {
            i--;
            continue;
        }

        switch (i) {
            case 0: {
                convertStringToArray(params.anchor_point, tmp_string);
                break;
            }
            case 1: {
                convertStringToArray(particle_count_in, tmp_string);
                for (int j = 0; j < 3; ++j) {
                    if (particle_count_in.at(j) < 0) {
                        spdlog::error("particle count {} smaller 0 is not allowed.", particle_count_in.at(j));
                    }
                    params.particle_dims.at(j) = static_cast<size_t>(particle_count_in.at(j));
                }
                break;
            }

            case 2: {
                convertStringToArray(params.velocity, tmp_string);
                break;
            }
            case 3: {
                const auto value = parseDouble(tmp_string);
                if (!value) {
                    spdlog::error("unexpected parameter {}, expected a double", i + 1);
                    std::exit(EXIT_FAILURE);
                    ;
                }
                params.spacing = *value;
                break;
            }
            case 4: {
                const auto value = parseDouble(tmp_string);
                if (!value) {
                    spdlog::error("unexpected parameter {}, expected a double", i + 1);
                    std::exit(EXIT_FAILURE);
                    ;
                }
                params.particle_mass = *value;
                break;
            }
            case 5: {
                const auto value = parseInt(tmp_string);
                if (!value) {
                    spdlog::error("unexpected parameter {}, expected an int", i + 1);
                    std::exit(EXIT_FAILURE);
                    ;
                }
                params.type = *value;
                break;
            }
            default: {
                // unreachable: i is always in [0, 5]
                spdlog::critical("unexpected parameter {}", i + 1);
                abort();
            }
        }
    }

    return params;
}

// Function to convert coordinates in a string in the form "x y z" to a double array
void ParticleCreator::convertStringToArray(std::array<double, 3> &array_to_fill, const std::string &line) {
    int index_to_fill = 0;
    std::string tmp = "";

    for (unsigned int i = 0; i < static_cast<unsigned int>(line.length()); i++) {
        if (line.at(i) == ' ' && tmp != "") {
            const auto value = parseDouble(tmp);
            if (!value) {
                spdlog::error("unexpected parameter {}", tmp);
                std::exit(EXIT_FAILURE);
                ;
            }
            array_to_fill.at(index_to_fill) = *value;

            index_to_fill += 1;
            tmp = "";
        }
        if (line.at(i) != ' ') {
            tmp += line.at(i);
        }
    }
    if (tmp != "") {
        const auto value = parseDouble(tmp);
        if (!value) {
            spdlog::error("unexpected parameter {}", tmp);
            std::exit(EXIT_FAILURE);
            ;
        }
        array_to_fill.at(index_to_fill) = *value;
    }
}

}  // namespace io
