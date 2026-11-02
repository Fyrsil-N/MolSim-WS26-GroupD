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

namespace io
{
    ParticleCreator::ParticleCreator(std::string filename) : config_filename(std::move(filename)) {}

    void ParticleCreator::readInParticles(ParticleContainer &particles)
    {
        if (std::ifstream input_file(config_filename); input_file.is_open())
        {
            std::string tmp_string;
            while (input_file.good() && !input_file.eof())
            {
                getline(input_file, tmp_string);

                if (tmp_string.empty() || tmp_string.at(0) == '#') continue;

                //
                if (tmp_string == "cuboid:")
                {
                    std::array<double, 3> start_coordinates{};
                    std::array<double, 3> end_coordinates{};
                    std::array<double, 3> total_velocity{};
                    double spacing = 0;
                    double total_mass = 0;
                    int index_value = 0;
                    while (input_file.good() && !input_file.eof())
                    {
                        //Retrieve next line
                        getline(input_file, tmp_string);

                        //ignore comments and empty lines
                        if (tmp_string.empty() || tmp_string.at(0) == '#') continue;
                        switch (index_value) {
                            case 0:
                                convertStringToArray(start_coordinates, tmp_string);
                                index_value++;
                                break;
                            case 1:
                                convertStringToArray(end_coordinates, tmp_string);
                                index_value++;
                                break;
                            case 2:
                                convertStringToArray(total_velocity, tmp_string);
                                index_value++;
                                break;
                            case 3:
                                spacing = stod(tmp_string);
                                index_value++;
                                break;
                            case 4:
                                total_mass = stod(tmp_string);
                                index_value++;
                                break;
                            default:
                                std::cerr << "To many arguments for rectangle" << std::endl;
                                break;
                        }
                        if (index_value == 5) {
                            break;
                        }
                    }
                    normalizeCoordinateArrays(start_coordinates, end_coordinates);
                    size_t x_particle_size = static_cast<int>((end_coordinates.at(0) - start_coordinates.at(0)) / spacing) + 1;
                    size_t y_particle_size = static_cast<int>((end_coordinates.at(1) - start_coordinates.at(1)) / spacing) + 1;
                    size_t z_particle_size = static_cast<int>((end_coordinates.at(2) - start_coordinates.at(2)) / spacing) + 1;
                    particles.reserve(x_particle_size * y_particle_size * z_particle_size);
                    for (unsigned int i = 0; i < x_particle_size; ++i) {
                        for (unsigned int j = 0; j < y_particle_size; ++j) {
                            for (unsigned int k = 0; k < z_particle_size; ++k) {
                                std::array<double, 3> pos = {
                                    start_coordinates.at(0) + static_cast<double>(i) * spacing,
                                    start_coordinates.at(1) + static_cast<double>(j) * spacing,
                                    start_coordinates.at(2) + static_cast<double>(k) * spacing
                                };
                                particles.emplaceBack(pos, total_velocity, total_mass);
                            }
                        }
                    }
                }
                else
                {
                    std::cerr << "Error: Form '" << tmp_string << "' unknown." << std::endl;
                }
            }
        }
        else {
            std::cerr << "Error: could not open file " << config_filename << std::endl;
            exit(-1);
        }
    }

    // Function to convert coordinates in a string in the form "x y z" to an integer array
    void ParticleCreator::convertStringToArray(std::array<double, 3> &array_to_fill, const std::string &line)
    {
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
    void ParticleCreator::normalizeCoordinateArrays(std::array<double, 3> &a, std::array<double, 3> &b) {
        std::array<double, 3> copy_a{};
        std::array<double, 3> copy_b{};
        std::ranges::copy(a, std::begin(copy_a));
        std::ranges::copy(b, std::begin(copy_b));
        a.at(0) = copy_a.at(0) < copy_b.at(0) ? copy_a.at(0) : copy_b.at(0);
        a.at(1) = copy_a.at(1) < copy_b.at(1) ? copy_a.at(1) : copy_b.at(1);
        a.at(2) = copy_a.at(2) < copy_b.at(2) ? copy_a.at(2) : copy_b.at(2);
        b.at(0) = copy_a.at(0) > copy_b.at(0) ? copy_a.at(0) : copy_b.at(0);
        b.at(1) = copy_a.at(1) > copy_b.at(1) ? copy_a.at(1) : copy_b.at(1);
        b.at(2) = copy_a.at(2) > copy_b.at(2) ? copy_a.at(2) : copy_b.at(2);
    }
    }  // namespace io
