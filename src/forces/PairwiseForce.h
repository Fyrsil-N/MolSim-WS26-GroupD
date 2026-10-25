//
// Created by hutj on 25.10.26.
//

#pragma once

#include <array>

#include "Particle.h"

namespace forces {

/**
 * This is an Interface to group all types of forces that act pairwise on two particles.
 */
class PairwiseForce {
 public:
  virtual ~PairwiseForce() = default;

  /**
   * General Interface to compute a pairwise force between two particles.
   * @param p1 particle 1
   * @param p2 particle 2
   * @return the force on p1 caused by p2 as a Vector in 3D space
   */
  virtual std::array<double, 3> calculateForce(const Particle &p1, const Particle &p2) const = 0;
};

}  // namespace forces
