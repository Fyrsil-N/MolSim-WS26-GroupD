//
// Created by fmathis on 02/11/2026.
//

#include "forces/LennardJones.h"

#include "utils/ArrayUtils.h"

namespace forces {

std::array<double, 3> LennardJones::calculateForce(const Particle &p1, const Particle &p2) const {
    const auto diff = p2.getX() - p1.getX();
    const double dist = p1.distanceTo(p2);
    const double well = 24 * epsilon / (dist * dist);
    const double comp = sigma / dist;
    return well * (pow(comp, 6) - 2 * pow(comp, 12)) * diff;
}

}  // namespace forces