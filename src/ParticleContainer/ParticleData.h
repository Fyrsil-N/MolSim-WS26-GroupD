#pragma once

#include <array>
#include <vector>

/**
 * Struct-of-arrays storage of all particle properties. The entries at index i of all vectors belong to the same
 * particle. Owned by a ParticleContainer and References by the ParticleReferences.
 */
struct ParticleData {
    /**
     * Positions of the particles (Nx3)
     */
    std::vector<std::array<double, 3>> positions{};

    /**
     * Velocities of the particles (Nx3)
     */
    std::vector<std::array<double, 3>> velocities{};

    /**
     * Forces effective on the particles
     */
    std::vector<std::array<double, 3>> forces{};

    /**
     * Force which was effective on th particles in the last time step
     */
    std::vector<std::array<double, 3>> old_forces{};

    /**
     * Masses of th particles
     */
    std::vector<double> masses{};

    /**
     * Type of the particles. Use it for whatever you want (e.g. to separate
     * molecules belonging to different bodies, matters, and so on)
     */
    std::vector<int> types{};
};
