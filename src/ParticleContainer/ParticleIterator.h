#pragma once

#include <cstddef>

#include "ConstParticleHandle.h"
#include "ParticleData.h"
#include "ParticleHandle.h"

/**
 * Mutable iterator over the particles of a ParticleContainer for range-based for loops.
 * Stores a ParticleHandle and moves its index along.
 * Careful: the ParticleHandle is returned directly so auto& p refers to a different particle each iteration.
 */
class ParticleIterator {
   private:
    /**
     * Reference to the current particle
     */
    ParticleHandle particle;

    /**
     * Constructor of the iterator. Only intended to be used with begin() end() in ParticleContainer.
     * @param data Pointer to the particle data to iterate over
     * @param index Index of the particle the iterator starts at
     */
    ParticleIterator(ParticleData *data, std::size_t index) : particle(data, index) {}

   public:
    /**
     * @return reference to the current particle
     */
    ParticleHandle &operator*() { return particle; }

    /**
     * Move to the next particle.
     * @return this iterator
     */
    ParticleIterator &operator++() {
        ++particle.index;
        return *this;
    }

    /**
     * @param other iterator to compare to
     * @return true if both iterators point to different particles
     */
    bool operator!=(const ParticleIterator &other) const { return particle.index != other.particle.index; }

    friend class ParticleContainer;
};

/**
 * Read only iterator over the particles of a ParticleContainer for range-based for loops.
 * Stores a ConstParticleHandle and moves its index along.
 * Careful: the ConstParticleHandle is returned directly so auto& p refers to a different particle each iteration.
 */
class ConstParticleIterator {
   private:
    /**
     * Reference to the current particle
     */
    ConstParticleHandle particle;

    /**
     * Constructor of the iterator. Only intended to be used with begin() end() in ParticleContainer
     * @param data Pointer to the particle data to iterate over
     * @param index Index of the particle the iterator starts at
     */
    ConstParticleIterator(const ParticleData *data, std::size_t index) : particle(data, index) {}

   public:
    /**
     * @return read-only reference to the current particle
     */
    const ConstParticleHandle &operator*() { return particle; }

    /**
     * Move to the next particle.
     * @return this iterator
     */
    ConstParticleIterator &operator++() {
        ++particle.index;
        return *this;
    }

    /**
     * @param other iterator to compare to
     * @return true if both iterators point to different particles
     */
    bool operator!=(const ConstParticleIterator &other) const { return particle.index != other.particle.index; }

    friend class ParticleContainer;
};
