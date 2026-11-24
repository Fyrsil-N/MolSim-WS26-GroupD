//
// Created by hutj on 24.11.26.
//

#pragma once

#include "ParticleContainer/ParticleContainer.h"
#include "io/input/ShapeParameters.h"

namespace io {
/**
 * This class implements methods to create and insert particles into a particle container arranged in different shapes.
 */
class ParticleGenerator {
   public:
    /**
     * Generates a cuboid based on given parameters.
     * @param params the shape parameters
     * @param particles ParticleContainer to store all the newly generated particles
     * @param third_dim Whether the third spatial dimension is enabled
     */
    static void generateCuboid(const ShapeParameters &params, ParticleContainer &particles, bool third_dim);

    /**
     * Generates a particle based on given parameters.
     * @param params the shape parameters
     * @param particles ParticleContainer to store all the newly generated particles
     * @param third_dim Whether the third spatial dimension is enabled
     */
    static void generateParticle(const ShapeParameters &params, ParticleContainer &particles, bool third_dim);

    /**
     * Generates an ellipsoid based on given parameters.
     * @param params the shape parameters
     * @param particles ParticleContainer to store all the newly generated particles
     * @param third_dim Whether the third spatial dimension is enabled
     */
    static void generateEllipsoid(const ShapeParameters &params, ParticleContainer &particles, bool third_dim);
};
}  // namespace io
