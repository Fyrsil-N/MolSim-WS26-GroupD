//
// Created by hutj on 25.10.26.
//

#include "Simulator.h"

#include <spdlog/spdlog.h>

#include "utils/ArrayUtils.h"

Simulator::Simulator(ParticleContainer &particles_arg, std::unique_ptr<forces::PairwiseForce> pairwise_force_arg,
                     double delta_t_arg, double t_end_arg, bool parallel_execution_arg [[maybe_unused]],
                     std::vector<std::unique_ptr<io::OutputWriter>> output_writers_arg)
    : particles(particles_arg),
      pairwise_force(std::move(pairwise_force_arg)),
      delta_t(delta_t_arg),
      t_end(t_end_arg),
      parallel_execution_enabled(parallel_execution_arg),
      output_writers(std::move(output_writers_arg)) {}

void Simulator::updateX() {
    for (auto &p : particles) {
        auto delta_x = delta_t * p.getV() + delta_t * delta_t / (2 * p.getM()) * p.getF();
        p.setX(p.getX() + delta_x);
    }
}

void Simulator::updateV() {
    for (auto &p : particles) {
        auto delta_v = delta_t / (2 * p.getM()) * (p.getOldF() + p.getF());
        p.setV(p.getV() + delta_v);
    }
}

void Simulator::runSimulation() {
    double current_time = 0;

    int iteration = 0;

    int total_iterations = static_cast<int>(round((t_end / delta_t)));

    // call updateF() once before the loop to set a real starting force and not 0 for the first iteration.
    particles.updateForce(*pairwise_force);
    // record the initial state of the simulation regardless of output interval
    for (auto &writer : output_writers) {
        writer->onStep(particles, iteration, current_time);
    }

    for (iteration = 1; iteration <= total_iterations; iteration++) {
        updateX();
        particles.updateForce(*pairwise_force);
        updateV();

        current_time = iteration * delta_t;

        for (auto &writer : output_writers) {
            writer->onStep(particles, iteration, current_time);
        }
    }

    for (auto &writer : output_writers) {
        writer->onEnd(particles);
    }
}