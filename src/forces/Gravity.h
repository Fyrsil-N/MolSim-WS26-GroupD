//
// Created by hutj on 25.10.26.
//

#pragma once
#include "PairwiseForce.h"
#include "utils/ArrayUtils.h"

namespace forces {

/**
 * This class implements the computation of gravitational attraction between two particles.
 */
class Gravity : public PairwiseForce {
   public:
    /**
     * Compute the gravitational force on p1 exerted by p2.
     * @param p1 particle 1
     * @param p2 particle 2
     * @return gravitational force on p1 exerted by p2 in 3D space
     */
    inline std::array<double, 3> calculateForce(const ConstParticleHandle p1,
                                                const ConstParticleHandle p2) const override {
        const auto diff = p2.getX() - p1.getX();
        const double dist = ArrayUtils::L2Norm(diff);
        return (p1.getM() * p2.getM() / (dist * dist * dist)) * diff;
    }
};

}  // namespace forces
