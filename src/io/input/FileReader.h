/*
 * FileReader.h
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */

#pragma once

#include "InputReader.h"
#include "ParticleContainer.h"

namespace io {

/**
 * This class implements functionality to read initial particle configurations out of an input file and create the
 * corresponding ParticleContainer and Particles.
 */
class FileReader : public InputReader {
   private:
    /**
     * Name of the input file that stores the initial particle data.
     */
    std::string input_filename;

   public:
    /**
     * Constructor for a FileReader for the given input file.
     * @param filename Name of the input file that stores the initial particle data
     */
    explicit FileReader(std::string filename);

    /**
     * Read in initial particle data from the input file of the class and store the new particles in the container.
     * @param particles ParticleContainer to store all the newly generated particles
     */
    void readInParticles(ParticleContainer &particles) override;
};

}  // namespace io
