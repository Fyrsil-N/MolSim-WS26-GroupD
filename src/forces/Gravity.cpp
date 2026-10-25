//
// Created by hutj on 25.10.26.
//

#include "Gravity.h"

#include "utils/ArrayUtils.h"

namespace forces {

std::array<double, 3> Gravity::calculateForce(const Particle &p1, const Particle &p2) const {
  const auto diff = p2.getX() - p1.getX();
  const double dist = ArrayUtils::L2Norm(diff);
  return (p1.getM() * p2.getM() / (dist * dist * dist)) * diff;
}

}  // namespace forces
