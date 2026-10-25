//
// Created by hutj on 25.10.26.
//

#pragma once
#include "PairwiseForce.h"

namespace forces {

class Gravity : public PairwiseForce {
 public:
  /**
   * Compute the gravitational force on p1 exerted by p2.
   * @param p1 particle 1
   * @param p2 particle 2
   * @return gravitational force on p1 exerted by p2 in 3D space
   */
  std::array<double, 3> calculateForce(const Particle &p1, const Particle &p2) const override;
};

}  // namespace forces
