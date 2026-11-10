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
        spdlog::error("could not open file '{}'", input_filename);
        exit(-1);
    }

    getline(input_file, tmp_string);
    spdlog::trace("Read line: {}", tmp_string);

    while (tmp_string.empty() or tmp_string[0] == '#') {
        getline(input_file, tmp_string);
        spdlog::trace("Read line: {}", tmp_string);
    }

    std::istringstream numstream(tmp_string);
    numstream >> num_particles;
    spdlog::debug("Reading {} particles from '{}'", num_particles, input_filename);
    getline(input_file, tmp_string);
    spdlog::trace("Read line: {}", tmp_string);
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
            spdlog::error("'{}': eof reached unexpectedly reading line {} of {}", input_filename, i + 1, num_particles);
            std::exit(EXIT_FAILURE);
        }
        datastream >> m;
        particles.emplaceBack(x, v, m);

        getline(input_file, tmp_string);
        spdlog::trace("Read line: {}", tmp_string);
    }
}

}  // namespace io
