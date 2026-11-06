//Header extension of  ParticleContainer.h to keep the original Header file clean
#pragma once

#include <omp.h>
#include <vector>
#include "Particle.h"
#include "utils/ArrayUtils.h"
template <class F>
void ParticleContainer::forEachPair(const F &f, const bool parallel_calculation) {
    if (parallel_calculation) {
        const int nthreads = omp_get_max_threads();
        const std::size_t n = particles.size();
        std::vector<std::vector<std::array<double, 3>>> thread_results(nthreads, std::vector<std::array<double, 3>>(n, {0.0, 0.0, 0.0}));
#pragma omp parallel
        {
            auto &local = thread_results.at(omp_get_thread_num());
#pragma omp for schedule(dynamic, 8)
            for (std::size_t i = 0; i < n; ++i) {
                for (std::size_t j = i + 1; j < n; ++j) {
                    std::array<double, 3> force = f(particles.at(i), particles.at(j));
                    local.at(i) = local.at(i) + force;
                    local.at(j) = local.at(j) - force;
                }
            }
        }
#pragma omp parallel for
        for (std::size_t j = 0; j < n; ++j) {
            for (int t = 0; t < nthreads; ++t) {
                particles.at(j).addToF(thread_results.at(t).at(j));
            }
        }
    }
    else {
        const std::size_t n = particles.size();
        for (std::size_t i = 0; i < n; ++i) {
            for (std::size_t j = i + 1; j < n; ++j) {
                std::array<double, 3> force = f(particles.at(i), particles.at(j));
                particles.at(i).addToF(force);
                particles.at(j).addToF((-1) * force);
            }
        }
    }
}