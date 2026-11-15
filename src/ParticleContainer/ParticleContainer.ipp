// Header extension of  ParticleContainer.h to keep the original Header file clean
#pragma once

#ifdef ENABLE_OPENMP
#include <omp.h>
#endif

#include <array>
#include <vector>

#include "Particle.h"
#include "utils/ArrayUtils.h"

template <class F>
void ParticleContainer::forEachPair(const F &f, [[maybe_unused]] const bool parallel_calculation) {
    const std::size_t n = particles.size();

#ifdef ENABLE_OPENMP
    if (parallel_calculation) {
        const int nthreads = omp_get_max_threads();
        std::vector<std::vector<std::array<double, 3>>> thread_results(
            nthreads, std::vector<std::array<double, 3>>(n, {0.0, 0.0, 0.0}));
#pragma omp parallel
        {
            auto &local = thread_results[omp_get_thread_num()];
#pragma omp for schedule(dynamic, 8)
            for (std::size_t i = 0; i < n; ++i) {
                for (std::size_t j = i + 1; j < n; ++j) {
                    std::array<double, 3> force = f(particles[i], particles[j]);
                    local[i] = local[i] + force;
                    local[j] = local[j] - force;
                }
            }
        }
#pragma omp parallel for
        for (std::size_t j = 0; j < n; ++j) {
            for (int t = 0; t < nthreads; ++t) {
                particles[j].addToF(thread_results[t][j]);
            }
        }
        return;
    }
#endif

    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = i + 1; j < n; ++j) {
            std::array<double, 3> force = f(particles[i], particles[j]);
            particles[i].addToF(force);
            particles[j].addToF((-1) * force);
        }
    }
}
