//
// Created by hutj on 23.11.26.
//

#pragma once

#include <array>
#include <initializer_list>

#include "ParticleContainer/DirectSumContainer.h"

/**
 * Small helpers that are useful in multiple tests to make them more readable.
 */
namespace testHelpers {

/**
 * Particles can only be created via a container so this helper creates a container and fills it with particles at the
 * given positions. Each particle gets it's own type so they can be told apart easily.
 * @param positions positions of the particles in insertion order
 * @return container holding the particles
 */
inline DirectSumContainer makeParticles(std::initializer_list<std::array<double, 3>> positions) {
    DirectSumContainer container;
    container.reserve(positions.size());
    int type = 0;
    for (const auto &position : positions) {
        container.emplaceBack(position, {0.0, 0.0, 0.0}, 1.0, type++);
    }
    return container;
}

}  // namespace testHelpers
