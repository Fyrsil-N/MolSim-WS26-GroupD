//
// Created by hutj on 08.11.26.
//

#include "PerformanceWriter.h"

#include <iostream>

#ifdef ENABLE_OPENMP
#include <omp.h>
#endif

namespace io {
PerformanceWriter::PerformanceWriter(bool parallel_execution [[maybe_unused]])
    : OutputWriter(1), number_of_iterations(0), start_cpu_time(0) {
#ifdef ENABLE_OPENMP
    if (parallel_execution) {
        thread_count = omp_get_max_threads();
    } else {
        thread_count = 1;
    }
#else
    thread_count = 1;
#endif
}

void PerformanceWriter::onStep(const ParticleContainer &particles [[maybe_unused]], int iteration,
                               double time [[maybe_unused]]) {
    if (iteration == 0) {
        start_time = std::chrono::steady_clock::now();
        start_cpu_time = std::clock();
    }

    number_of_iterations = iteration;
}

void PerformanceWriter::onEnd(const ParticleContainer &particles) {
    const std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
    const std::clock_t end_cpu_time = std::clock();
    const double total_simulation_time = std::chrono::duration<double>(end_time - start_time).count();

    // std::clock counts the cpu time of the whole process (all threads combined)
    const double cpu_simulation_time = static_cast<double>(end_cpu_time - start_cpu_time) / CLOCKS_PER_SEC;

    const double time_per_iteration = total_simulation_time / static_cast<double>(number_of_iterations);

    const double particle_updates_per_second =
        static_cast<double>(particles.size() * number_of_iterations) / total_simulation_time;

    std::cout << "Final Statistics of the simulation run: \n"
              << "\n"
              << "  Total simulation time                 " << total_simulation_time << " s\n"
              << "  Number of iterations                  " << number_of_iterations << "\n"
              << "  Simulation time per iteration         " << time_per_iteration << " s\n"
              << "\n"
              << "  Number of simulated particles         " << particles.size() << "\n"
              << "  Particle updates per second           " << particle_updates_per_second << "\n"
              << "\n"
              << "  Number of Threads used                " << thread_count << "\n"
              << "  CPU Time                              " << cpu_simulation_time << " s\n";
}

}  // namespace io
