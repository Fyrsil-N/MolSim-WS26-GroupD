//
// Created by fmathis on 05/11/2026.
//

#pragma once

#include <array>

/**
 * Parameters for shape creation
 */
struct ShapeParameters {
    /**
     * Point to anchor the shape to
     */
    std::array<double, 3> anchor_point{};

    /**
     * Dimensions of the shape in particles
     */
    std::array<size_t, 3> particle_dims{};

    /**
     * Initial velocity of the generated particles
     */
    std::array<double, 3> velocity{};

    /**
     * Spacing between the particles
     */
    double spacing = 1;

    /**
     * Mass of each particle
     */
    double particle_mass = 1;

    /**
     * type of each particle
     */
    int type = 0;
};
