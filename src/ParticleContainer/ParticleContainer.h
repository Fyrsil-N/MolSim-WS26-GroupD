//
// Created by hutj on 15.11.26.
//

#pragma once

#include <array>
#include <cassert>
#include <cstddef>

#include "ConstParticleHandle.h"
#include "ParticleData.h"
#include "ParticleHandle.h"
#include "ParticleIterator.h"
#include "forces/PairwiseForce.h"

/**
 * This class implements the basic functionality of storing, reading and modifying arbitrary particles.
 * Particles cannot be accessed directly anymore, only via the provided interfaces (Particle Class doesn't exist
 * anymore)
 */
class ParticleContainer {
   protected:
    /**
     * Struct-of-arrays storage of all particles
     */
    ParticleData data;

    /** Default constructor */
    ParticleContainer() = default;

    /** Default Copy constructor
     * @param other container to copy from
     */
    ParticleContainer(const ParticleContainer &other) = default;

    /**
     * Default Move constructor
     * @param other container to move from
     */
    ParticleContainer(ParticleContainer &&other) noexcept = default;

    /**
     * Default Copy Assignment
     * @param other container to copy from
     * @return this container
     */
    ParticleContainer &operator=(const ParticleContainer &other) = default;

    /**
     * Default Move assignment
     * @param other container to move from
     * @return this container
     */
    ParticleContainer &operator=(ParticleContainer &&other) noexcept = default;

   public:
    /** Default Virtual destructor (needed for abstract classes) */
    virtual ~ParticleContainer() = default;

    /**
     * Reserve storage for n particles ahead of their entry in storage to avoid reallocations
     * @param n The amount of particles to reserve space for.
     */
    void reserve(std::size_t n);

    /**
     * Attempt to free any unused reserved storage
     */
    void shrinkToFit();

    /**
     * Creates a particle  and inserts it at the end of the Container (mirrors old Particle Constructor)
     * @param x_arg initial (x, y, z) position of the particle
     * @param v_arg initial velocity in 3D space
     * @param m_arg weight of the particle
     * @param type field to distinguish between different particle types
     */
    void emplaceBack(const std::array<double, 3> &x_arg, const std::array<double, 3> &v_arg, double m_arg,
                     int type = 0);

    /**
     * @return Number of particles in the Container
     */
    std::size_t size() const;

    /**
     * @return True if the container is empty
     */
    bool empty() const;

    /**
     * Access to single particle via a ParticleHandle on its index.
     * @param i index of the particle
     * @return handle to read and modify the particle
     */
    ParticleHandle getHandle(std::size_t i) {
        assert(i < size());
        return {&data, i};
    }

    /**
     * Read-only access to a single particle via a ConstParticleHandle on its index.
     * @param i index of the particle
     * @return handle to read the particle
     */
    ConstParticleHandle getConstHandle(std::size_t i) const {
        assert(i < size());
        return {&data, i};
    }

    /** @return Iterator to the first particle */
    ParticleIterator begin() { return {&data, 0}; }
    /** @return Iterator past the last particle */
    ParticleIterator end() { return {&data, this->size()}; }
    /** @return Read-only iterator to the first particle */
    ConstParticleIterator begin() const { return {&data, 0}; }
    /** @return Read-only iterator past the last particle */
    ConstParticleIterator end() const { return {&data, this->size()}; }

    /**
     * Move f into old_f and start a new force calculation step with f = {0.0, 0.0, 0.0}
     */
    void resetForces();

    /**
     * Iterate over every unique pair of particles in the container and apply f(p_i, p_j).
     * @param force Callable taking two Particle references
     */
    virtual void updateForce(const forces::PairwiseForce &force) = 0;
};
