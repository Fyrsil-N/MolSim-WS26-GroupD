//
// Created by hutj on 22.10.26.
//

#pragma once

#include <array>
#include <cstddef>

#include "ParticleContainer.h"

/**
 * This version of the ParticleContainer implements calculating the forces for the particles via the direct n^2 loop
 */
class ParticleContainerDirectSum : public ParticleContainer {
   public:
    /**
     * Calculate the forces on the particles in the next timestep.
     * First move force into old_force, then compute the pairwise force between every unique particle pair and add them
     * together.
     * @param force PairwiseForce that acts between all pairs of unique particles
     */
    void updateForce(const forces::PairwiseForce &force) override {
        const std::size_t n = size();

        resetForces();

        // compute all pairwise forces and add them to each particles force
        // optimization: set both particles force at the same time (newtons third law)
        // The force calculation was moved to the ParticleContainer Header to allow for parallel force calculation
        for (std::size_t i = 0; i < n; ++i) {
            for (std::size_t j = i + 1; j < n; ++j) {
                std::array<double, 3> delta_f = force.calculateForce(getConstHandle(i), getConstHandle(j));
                data.forces[i] = data.forces[i] + delta_f;
                data.forces[j] = data.forces[j] - delta_f;
            }
        }
    }
};