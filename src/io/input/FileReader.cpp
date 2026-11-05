/*
 * FileReader.cpp
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */

#include "io/input/FileReader.h"

#include <spdlog/spdlog.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

#include "ParticleContainer.h"

namespace io {

FileReader::FileReader(std::string filename) : input_filename(std::move(filename)) {}

void FileReader::readInParticles(ParticleContainer &particles) {
    std::array<double, 3> x{};
    std::array<double, 3> v{};
    double m{};
    int num_particles = 0;

    std::ifstream input_file(input_filename);
    std::string tmp_string;

    if (!input_file.is_open()) {
        // Used error here due to the original error function
        spdlog::error("Error: could not open file " + input_filename);
        exit(-1);
    }

    getline(input_file, tmp_string);
    // Used info here, since this information is not critical to the user
    spdlog::info("Read line: " + tmp_string);

    while (tmp_string.empty() or tmp_string[0] == '#') {
        getline(input_file, tmp_string);
        // Used info here, since this information is not critical to the user
        spdlog::info("Read line: " + tmp_string);
    }

    std::istringstream numstream(tmp_string);
    numstream >> num_particles;
    // Used info here, since this information is not critical to the user
    spdlog::info("Reading " + std::to_string(num_particles) + ".");
    getline(input_file, tmp_string);
    spdlog::info("Read line: " + tmp_string);
    // reserve the space needed before hand to reduce moves of the ParticleContainer
    if (num_particles > 0) {
        particles.reserve(particles.size() + num_particles);
    }

    for (int i = 0; i < num_particles; i++) {
        std::istringstream datastream(tmp_string);

        for (auto &xj : x) {
            datastream >> xj;
        }
        for (auto &vj : v) {
            datastream >> vj;
        }
        if (datastream.eof()) {
            // Used error here due to the original error function
            spdlog::error("Error reading file: eof reached unexpectedly reading from line " + std::to_string(i));
            exit(-1);
        }
        datastream >> m;
        particles.emplaceBack(x, v, m);

        getline(input_file, tmp_string);
        // Used info here, since this information is not critical to the user
        spdlog::info("Read line: " + tmp_string);
    }
}

}  // namespace io
