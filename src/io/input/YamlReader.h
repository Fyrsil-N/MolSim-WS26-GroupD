//
// Created by fmathis on 13/11/2026.
//

#pragma once

#include <string>

#include "InputReader.h"

namespace io {

/**
 * Reads the simulation parameters and the initial particles (cuboids, ellipsoids, single particles) from a YAML file.
 */
class YamlReader : public InputReader {
    /**
     * Name of the configuration file that stores the input data.
     */
    std::string filename;

   public:
    /**
     * Constructor for a YamlReader for the given config file. Checks that the input file is parsable and has a
     * simulation and scene section as those are required for the other methods.
     * @param filename Name of the config file that stores the simulation and particle configuration
     * @throws io::InputError
     */
    explicit YamlReader(std::string filename);

    /**
     * Read in the `simulation` section: time step, end time, output settings, domain and pairwise force
     * @param params SimulationParameters to fill out
     * @throws io::InputError
     */
    void readInSimulation(SimulationParameters &params) override;

    /**
     * Read in the `scene` section and store the generated particles in the container.
     * @param particles ParticleContainer to store all the newly generated particles
     * @param third_dim Whether the third spatial dimension is enabled
     * @throws io::InputError
     */
    void readInParticles(ParticleContainer &particles, bool third_dim) override;
};
}  // namespace io
