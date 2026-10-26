//
// Created by hutj on 25.10.26.
//

#pragma once

#include <memory>
#include <vector>

#include "ParticleContainer.h"
#include "forces/PairwiseForce.h"
#include "io/output/OutputWriter.h"

/**
 * This class implements all physics and particle updates for the particles of the simulation.
 * It owns the entire simulation run which can be started with runSimulation().
 * As the particles get updates in place rerunning the simulation again will NOT produce the same results.
 */
class Simulator {
   private:
    /**
     * Particle Container with all the particles to be considered in the simulation
     */
    ParticleContainer &particles;
    /**
     * The force acting on each pair of particles
     */
    std::unique_ptr<forces::PairwiseForce> pairwise_force;
    /**
     * Relative time increase between two iteration of the simulation loop
     */
    double delta_t;
    /**
     * End time of the Simulation which starts at time = 0
     */
    double t_end;
    /**
     * List of output writers to be invokes during the simulation
     */
    std::vector<std::unique_ptr<io::OutputWriter>> output_writers;

    /**
     * calculate the force for all particles and update it in place
     */
    void updateF();

    /**
     * calculate the position for all particles and update it in place
     */
    void updateX();

    /**
     * calculate the velocity for all particles and update it in place
     */
    void updateV();

   public:
    /**
     * Initialize the Simulator
     * @param particles Particle Container with all the particles to be considered in the simulation
     * @param pairwise_force The force acting on each pair of particles
     * @param delta_t Relative time increase between two iteration of the simulation loop
     * @param t_end End time of the Simulation which starts at time = 0
     * @param output_writers List of output writers to be invoked during the simulation
     */
    Simulator(ParticleContainer &particles, std::unique_ptr<forces::PairwiseForce> pairwise_force, double delta_t,
              double t_end, std::vector<std::unique_ptr<io::OutputWriter>> output_writers);

    /**
     * Runs the entire Simulation loop from t_start = 0 to t_end at steps of delta_t.
     * Invokes the OutputWriters.onStep() at every iteration of the simulation and their onEnd() once at the end.
     * Important: this method should only be called ONCE as it changes the state of the particles, so two calls of the
     * Simulation are NOT the same simulation!
     */
    void runSimulation();
};
