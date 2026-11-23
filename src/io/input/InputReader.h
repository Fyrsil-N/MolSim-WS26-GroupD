//
// Created by hutj on 25.10.26.
//

#pragma once

#include "ParticleContainer/ParticleContainer.h"
#include "SimulationParameters.h"

namespace io {
/**
 * This class defines a unified interface for all types of input methods to use.
 */
class InputReader {
   public:
    virtual ~InputReader() = default;

    /**
     * Read in global simulation settings like domain size, output interval, start and end time
     * @param params SimulationParameters to fill out
     * @throws io::InputError
     */
    virtual void readInSimulation(SimulationParameters &params) = 0;

    /**
     * Read in initial particle data from the input file of the class and store the new particles in the container.
     * @param particles ParticleContainer to store all the newly generated particles
     * @param third_dim Whether the third spatial dimension is enabled
     * @throws io::InputError
     */
    virtual void readInParticles(ParticleContainer &particles, bool third_dim) = 0;
};
}  // namespace io
