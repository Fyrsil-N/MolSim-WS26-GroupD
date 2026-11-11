//
// Created by fmathis on 02/11/2026.
//

#pragma once
#include "PairwiseForce.h"

namespace forces {

/**
 * This class implements the computation of Lennard Jones forces between two particles.
 */
class LennardJones : public PairwiseForce {
   protected:
    double sigma = 1;
    double epsilon = 5;

   public:
    /**
     * Compute the Lennard-Jones potential on p1 exerted by p2.
     * @param p1 particle 1
     * @param p2 particle 2
     * @return Lennard-Jones potential on p1 exerted by p2 in 3D space
     */
    std::array<double, 3> calculateForce(const Particle &p1, const Particle &p2) const override;
};

}  // namespace forces
