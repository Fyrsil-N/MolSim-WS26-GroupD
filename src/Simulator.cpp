//
// Created by hutj on 25.10.26.
//

#include "Simulator.h"

#include "spdlog/spdlog.h"
#include "utils/ArrayUtils.h"

Simulator::Simulator(ParticleContainer &particles_arg, std::unique_ptr<forces::PairwiseForce> pairwise_force_arg,
                     double delta_t_arg, double t_end_arg,
                     std::vector<std::unique_ptr<io::OutputWriter>> output_writers_arg)
    : particles(particles_arg),
      pairwise_force(std::move(pairwise_force_arg)),
      delta_t(delta_t_arg),
      t_end(t_end_arg),
      output_writers(std::move(output_writers_arg)) {}

void Simulator::updateF() {
    // move f into old_f and start a new force calculation step with f = {0.0, 0.0, 0.0}
    for (auto &p : particles) {
        p.resetF();
    }
    // compute all pairwise forces and add them to each particles force
    // optimization: set both particles force at the same time (newtons third law)
    auto helper = [this](Particle &p1, Particle &p2) {
        auto force = pairwise_force->calculateForce(p1, p2);
        p1.addToF(force);
        p2.addToF((-1) * force);
    };
    particles.forEachPair(helper);
}

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

    int total_iterations = static_cast<int>(t_end/delta_t);

    spdlog::info("Simulating now for {0:d} iterations.", total_iterations);

    u_int8_t old_progress = 0;
    u_int8_t progress = 0;

    // call updateF() once before the loop to set a real starting force and not 0 for the first iteration.
    updateF();
    // record the initial state of the simulation regardless of output interval
    for (auto &writer : output_writers) {
        writer->onStep(particles, iteration, current_time);
    }

    for (iteration = 1; iteration <= total_iterations; iteration++) {
        updateX();
        updateF();
        updateV();

        current_time = iteration * delta_t;

        for (auto &writer : output_writers) {
            writer->onStep(particles, iteration, current_time);
        }

#if CMAKE_BUILD_TYPE == TRUE // The need for speed
        progress = 100 * iteration / total_iterations;
        if (progress >= old_progress + 10) {
            old_progress = progress;
            spdlog::info("{0:d}%", progress);
        }
#endif
    }

    for (auto &writer : output_writers) {
        writer->onEnd(particles);
    }
}