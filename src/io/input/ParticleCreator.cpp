//
// Created by luka-maric on 31.10.26.
//

#include "io/input/ParticleCreator.h"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <utility>

#include "ParticleContainer.h"
#include "utils/ArrayUtils.h"
#include "utils/MaxwellBoltzmannDistribution.h"

namespace io {

ParticleCreator::ParticleCreator(std::string filename) : config_filename(std::move(filename)) {}

void ParticleCreator::readInParticles(ParticleContainer &particles) {
    std::ifstream input_file(config_filename);

    if (!input_file.is_open()) {
        std::cerr << "Error: could not open file " << config_filename << std::endl;
        exit(-1);
    }
    std::string tmp_string;
    while (input_file.good() && !input_file.eof()) {
        getline(input_file, tmp_string);

        if (tmp_string.empty() || tmp_string.at(0) == '#') continue;

        if (tmp_string != "cuboid:") {
            std::cerr << "Error: Form '" << tmp_string << "' unknown." << std::endl;
            continue;
        }

        std::array<double, 3> start_coordinates{}, end_coordinates{}, total_velocity{};
        double spacing = 0;
        double total_mass = 0;

        for (int i = 0; i < 5; ++i) {
            if (!input_file.good() || input_file.eof()) {
                std::cerr << "Error reading file: " << config_filename << std::endl;
                exit(-1);
            }

            // Retrieve next line
            getline(input_file, tmp_string);

            // ignore comments and empty lines
            if (tmp_string.empty() || tmp_string.at(0) == '#') {
                i--;
                continue;
            }

            switch (i) {
                case 0:
                    convertStringToArray(start_coordinates, tmp_string);
                    break;
                case 1:
                    convertStringToArray(end_coordinates, tmp_string);
                    break;
                case 2:
                    convertStringToArray(total_velocity, tmp_string);
                    break;
                case 3:
                    spacing = stod(tmp_string);
                    break;
                case 4:
                    total_mass = stod(tmp_string);
                    break;
                default:
                    std::cerr << "How did you even reach this???" << std::endl;
                    abort();
            }
        }

        size_t x_particle_size =
            static_cast<int>(std::abs(end_coordinates.at(0) - start_coordinates.at(0)) / spacing) + 1;
        size_t y_particle_size =
            static_cast<int>(std::abs(end_coordinates.at(1) - start_coordinates.at(1)) / spacing) + 1;
        size_t z_particle_size =
            static_cast<int>(std::abs(end_coordinates.at(2) - start_coordinates.at(2)) / spacing) + 1;
        particles.reserve(x_particle_size * y_particle_size * z_particle_size);
        for (unsigned int x = 0; x < x_particle_size; ++x) {
            for (unsigned int y = 0; y < y_particle_size; ++y) {
                for (unsigned int z = 0; z < z_particle_size; ++z) {
                    std::array<double, 3> pos = {start_coordinates.at(0) + static_cast<double>(x) * spacing,
                                                 start_coordinates.at(1) + static_cast<double>(y) * spacing,
                                                 start_coordinates.at(2) + static_cast<double>(z) * spacing};
                    particles.emplaceBack(pos, total_velocity + maxwellBoltzmannDistributedVelocity(0.1, 2),
                                          total_mass);
                }
            }
        }
    }
}

// Function to convert coordinates in a string in the form "x y z" to an integer array
void ParticleCreator::convertStringToArray(std::array<double, 3> &array_to_fill, const std::string &line) {
    int index_to_fill = 0;
    std::string tmp = "";

    for (unsigned int i = 0; i < static_cast<unsigned int>(line.length()); i++) {
        if (line.at(i) == ' ' && tmp != "") {
            array_to_fill.at(index_to_fill) = stod(tmp);
            index_to_fill += 1;
            tmp = "";
        }
        if (line.at(i) != ' ') {
            tmp += line.at(i);
        }
    }
    if (tmp != "") {
        array_to_fill.at(index_to_fill) = stod(tmp);
    }
}

}  // namespace io
