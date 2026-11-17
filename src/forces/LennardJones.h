//
// Created by fmathis on 02/11/2026.
//

#pragma once
#include "PairwiseForce.h"
#include "utils/ArrayUtils.h"

namespace forces {

/**
 * This class implements the computation of Lennard Jones forces between two particles.
 */
class LennardJones : public PairwiseForce {
   protected:
    double sigma = 1;
    double epsilon = 5;
    double cutoff = 3;

   public:
    /**
     * Default constructor
     */
    LennardJones() = default;

    /**
     * Constructor allowing force parameterization
     * @param sigma Distance where LJ(sigma) = 0
     * @param epsilon Depth of potential well
     * @param cutoff Distance cutoff for the calculation
     */
    LennardJones(const double sigma, const double epsilon, const double cutoff)
        : sigma(sigma), epsilon(epsilon), cutoff(cutoff) {}

    /**
     * Compute the Lennard-Jones force on p1 exerted by p2.
     * @param p1 particle 1
     * @param p2 particle 2
     * @return Lennard-Jones potential on p1 exerted by p2 in 3D space
     */
    inline std::array<double, 3> calculateForce(const ConstParticleHandle p1,
                                                const ConstParticleHandle p2) const override {
        // @TODO make this function inlinable (currently the interface blocks that)
        // @TODO make the cutoff radius check optional for the direct sum implementation
        const auto diff = p2.getX() - p1.getX();
        const double r2 = ArrayUtils::squaredL2Norm(diff);

        if (r2 > cutoff * cutoff) {
            return {0, 0, 0};
        }

        // power calls are expensive so use simple multiplication instead
        const double sigma2_over_r2 = sigma * sigma * (1 / r2);
        const double sigma6_over_r6 = sigma2_over_r2 * sigma2_over_r2 * sigma2_over_r2;

        return 24 * epsilon * (1 / r2) * (sigma6_over_r6 - 2 * sigma6_over_r6 * sigma6_over_r6) * diff;
    }
};

}  // namespace forces
