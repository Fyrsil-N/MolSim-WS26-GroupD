//
// Created by hutj on 08.11.26.
//

#pragma once
#include "OutputWriter.h"

namespace io {
/**
 * This class logs how far the simulation has progressed in steps of 10%.
 */
class ProgressWriter : public OutputWriter {
   private:
    /**
     * Total number of iterations the simulation will run for
     */
    int total_iterations;

    /**
     * Last progress in percent that was logged
     */
    int old_progress;

   public:
    /**
     * Constructor for the ProgressWriter
     * @param total_iterations_arg Total number of iterations the simulation will run for
     */
    explicit ProgressWriter(int total_iterations_arg);

    /**
     * Output the progress in percent the simulation has already run in 10% steps
     * @param particles Particles used in the simulation (unused)
     * @param iteration Current iteration number
     * @param time Current in-simulation time (unused)
     */
    void onStep(const ParticleContainer &particles [[maybe_unused]], int iteration,
                double time [[maybe_unused]]) override;
};
}  // namespace io
