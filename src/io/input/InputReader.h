//
// Created by hutj on 25.10.26.
//

#pragma once

#include "SimulationParameters.h"

namespace io {
/**
 * This class defines a unified interface for all types of input methods to use.
 */
class InputReader {
   public:
    virtual ~InputReader() = default;

    /**
     * Read in all data necessary for simulation from the input file
     * Replaces readInSimulation and readInParticles
     * @param params SimulationParameters struct to fill
     */
    virtual void readInput(SimulationParameters &params) = 0;
};
}  // namespace io
