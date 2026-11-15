#pragma once

#include <array>
#include <cstddef>

#include "ParticleData.h"

/**
 * This class provides efficient particle wise getters to the ParticleData.
 * This class can be used in const iterators as it cannot write to the ParticleContainer.
 * Should never be instantiated from outside and is only intended to be used in for-range loops.
 */
class ConstParticleHandle {
   protected:
    /**
     * Pointer to the particle data into which this object references
     */
    const ParticleData *const_data;

    /**
     * Index of the referenced Particle
     */
    std::size_t index;

    /**
     * Constructor of the ParticleHandle.
     * @param data Pointer to the particle data into which this object references
     * @param index Index of the referenced Particle
     */
    ConstParticleHandle(const ParticleData *data, std::size_t index) : const_data(data), index(index) {}

   public:
    /**
     * Get the position of the particle.
     * @return the position in 3D space (x, y, z)
     */
    std::array<double, 3> getX() const { return const_data->positions[index]; }

    /**
     * Get the velocity of the particle.
     * @return the velocity in 3D space
     */
    std::array<double, 3> getV() const { return const_data->velocities[index]; }

    /**
     * Get the current force on the particle.
     * @return the force in 3D space
     */
    std::array<double, 3> getF() const { return const_data->forces[index]; }

    /**
     * Get the force on the particle in the old time step.
     * @return the force in 3D space
     */
    std::array<double, 3> getOldF() const { return const_data->old_forces[index]; }

    /**
     * Get the mass of the particle.
     * @return the mass
     */
    double getM() const { return const_data->masses[index]; }

    /**
     * Get the type of the particle.
     * @return the type
     */
    int getType() const { return const_data->types[index]; }

    friend class ConstParticleIterator;
    friend class ParticleContainer;
};
