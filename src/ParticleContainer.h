//
// Created by hutj on 22.10.26.
//

#pragma once

#include <vector>

#include "Particle.h"

/**
 * ParticleContainer stores all the particles of the simulation in an efficient way.
 * It provides both iterators over individual particles and an interface to iterate over all particle pairs.
 */
class ParticleContainer {
   private:
    /**
     * Storage of all the particles
     */
    std::vector<Particle> particles;

   public:
    /** Mutable iterator over the particles. */
    using iterator = std::vector<Particle>::iterator;
    /** Read-only iterator over the particles. */
    using const_iterator = std::vector<Particle>::const_iterator;

    /**
     * Reserve storage for n particles ahead of their entry in storage to avoid reallocations
     * @param n The amount of particles to reserve space for.
     */
    void reserve(std::size_t n);

    /**
     * Create a new particle at the end of the container.
     * @param args Arguments for the particle constructor
     * @return Reference to the new particle
     */
    template <class... Args>
    Particle &emplaceBack(Args &&...args) {
        return particles.emplace_back(std::forward<Args>(args)...);
    }

    /**
     * @return Number of particles in the Container
     */
    std::size_t size() const;

    /**
     * @return True if the container is empty
     */
    bool empty() const;

    /** @return Iterator to the first particle */
    iterator begin();
    /** @return Iterator past the last particle */
    iterator end();
    /** @return Read-only iterator to the first particle */
    const_iterator begin() const;
    /** @return Read-only iterator past the last particle */
    const_iterator end() const;

    /**
     * Iterate over every unique pair of particles in the container and apply f(p_i, p_j).
     * @param f Callable taking two Particle references
     */
    template <class F>
    void forEachPair(const F &f) {
        for (std::size_t i = 0; i < particles.size(); ++i) {
            for (std::size_t j = i + 1; j < particles.size(); ++j) {
                f(particles.at(i), particles.at(j));
            }
        }
    }

    /**
     * @return human readable representation of a ParticleContainer
     */
    std::string toString() const;
};