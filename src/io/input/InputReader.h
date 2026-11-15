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
     * Read in global simulation setting like domain size, output interval, start and end time
     * @param params SimulationParameters for the simulator
     */
    virtual void readInSimulation(SimulationParameters &params) { (void)params; }

    /**
     * Read in initial particle data from some source, create particles from it and store them into the given Container.
     * @param particles ParticleContainer to store all the newly generated particles
     */
    virtual void readInParticles(ParticleContainer &particles) = 0;
};
}  // namespace io
