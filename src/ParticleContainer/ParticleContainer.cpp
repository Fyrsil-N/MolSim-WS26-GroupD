//
// Created by hutj on 15.11.26.
//

#include "ParticleContainer.h"

#include <algorithm>

void ParticleContainer::reserve(std::size_t n) {
    data.positions.reserve(n);
    data.velocities.reserve(n);
    data.forces.reserve(n);
    data.old_forces.reserve(n);
    data.masses.reserve(n);
    data.types.reserve(n);
}
void ParticleContainer::shrinkToFit() {
    data.positions.shrink_to_fit();
    data.velocities.shrink_to_fit();
    data.forces.shrink_to_fit();
    data.old_forces.shrink_to_fit();
    data.masses.shrink_to_fit();
    data.types.shrink_to_fit();
}

void ParticleContainer::emplaceBack(const std::array<double, 3> &x_arg, const std::array<double, 3> &v_arg,
                                    double m_arg, int type) {
    data.positions.emplace_back(x_arg);
    data.velocities.emplace_back(v_arg);
    data.forces.emplace_back(std::array<double, 3>{0.0, 0.0, 0.0});
    data.old_forces.emplace_back(std::array<double, 3>{0.0, 0.0, 0.0});
    data.masses.emplace_back(m_arg);
    data.types.emplace_back(type);
}

std::size_t ParticleContainer::size() const {
    // The invariant that all component have the same size is given as creating particles only happens through
    // emplaceBack and that adds an entry to all six vectors so they wont fall out of sync
    return data.positions.size();
}

bool ParticleContainer::empty() const { return size() == 0; }

void ParticleContainer::resetForces() {
    std::swap(data.old_forces, data.forces);
    std::fill(data.forces.begin(), data.forces.end(), std::array<double, 3>{0.0, 0.0, 0.0});
}