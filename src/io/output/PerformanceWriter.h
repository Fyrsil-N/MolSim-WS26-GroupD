//
// Created by hutj on 08.11.26.
//

#pragma once
#include <chrono>
#include <ctime>

#include "OutputWriter.h"

namespace io {

/**
 * This class implements functionality to measure the speed of a simulation run and display a comprehensive summary
 * afterwards (number of threads, number of iterations, total simulation time, cpu simulation time, time per iteration).
 */
class PerformanceWriter : public OutputWriter {
   private:
    /**
     * number of threads the force calculation runs on (1 for a serial run).
     */
    int thread_count;

    /**
     * number of iterations the simulation took
     */
    int number_of_iterations;

    /**
     * start time of the simulation set on first onStep call (misses the first force calculation)
     */
    std::chrono::steady_clock::time_point start_time;

    /**
     * cpu time the process had used at the start of the simulation set on first onStep call
     */
    std::clock_t start_cpu_time;

   public:
    /**
     * Constructor for the Performance Writer
     * @param parallel_execution True if the forces are calculated in parallel
     */
    explicit PerformanceWriter(bool parallel_execution);

    /**
     * Record the statistics for the current iteration. No output will be displayed.
     * @param particles Particles used in the simulation
     * @param iteration Current iteration number
     * @param time Current in-simulation time
     */
    void onStep(const ParticleContainer &particles, int iteration, double time) override;

    /**
     * Final computation of all statistics and output to the console in a readable format.
     * std::cout is used because all types of logging are disabled for performance measurements.
     * @param particles Particles used in the simulation
     */
    void onEnd(const ParticleContainer &particles) override;
};
}  // namespace io