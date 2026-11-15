#pragma once

#include <array>
#include <cstddef>

#include "ConstParticleHandle.h"
#include "ParticleData.h"
#include "utils/ArrayUtils.h"

/**
 * This class provides efficient particle wise getters and setters to the ParticleContainer.
 * This class can be used in mutable iterators to change the ParticleData.
 * Should never be instantiated from outside and is only intended to be used in for-range loops.
 */
class ParticleHandle : public ConstParticleHandle {
   protected:
    /**
     * Non const pointer to the particle data this class references.
     * Needed because this class implements setters which change the content of the underlying container.
     */
    ParticleData *mutable_data;

    /**
     * Constructor of the ParticleHandle.
     * @param data Pointer to the particle data into which this object references
     * @param index Index of the referenced Particle
     */
    ParticleHandle(ParticleData *data, std::size_t index) : ConstParticleHandle(data, index), mutable_data(data) {}

   public:
    /**
     * Set the position of the particle.
     * @param x new position
     */
    void setX(const std::array<double, 3> &x) { mutable_data->positions[index] = x; }

    /**
     * Set the velocity of the particle.
     * @param v new velocity
     */
    void setV(const std::array<double, 3> &v) { mutable_data->velocities[index] = v; }

    /**
     * Set the force on the particle.
     * @param f new force
     */
    void setF(const std::array<double, 3> &f) { mutable_data->forces[index] = f; }

    /**
     * Set the force on the particle in the old time step.
     * @param old_f new force
     */
    void setOldF(const std::array<double, 3> &old_f) { mutable_data->old_forces[index] = old_f; }

    friend class ParticleIterator;
    friend class ParticleContainer;
};
