//
// Created by hutj on 25.10.26.
//

#pragma once
#include "ParticleContainer.h"

/**
 * Namespace io groups all interfaces and classes used to read in particle data at the start of the simulation and used
 * to output the state of the simulation.
 */
namespace io {
/**
 * This class defines a unified interface for all types of input methods to use.
 */
class InputReader {
   public:
    virtual ~InputReader() = default;

    /**
     * Read in initial particle data from some source, create particles from it and store them into the given Container.
     * @param particles ParticleContainer to store all the newly generated particles
     */
    virtual void readInParticles(ParticleContainer &particles) = 0;
};
}  // namespace io
