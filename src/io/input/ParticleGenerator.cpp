//
// Created by hutj on 24.11.26.
//

#include "ParticleGenerator.h"

#include <spdlog/spdlog.h>

#include <array>

#include "utils/ArrayUtils.h"
#include "utils/MaxwellBoltzmannDistribution.h"

namespace io {
void ParticleGenerator::generateCuboid(const ShapeParameters &params, ParticleContainer &particles,
                                       const bool third_dim) {
    particles.reserve(particles.size() +
                      params.particle_dims.at(0) * params.particle_dims.at(1) * params.particle_dims.at(2));
    for (size_t x = 0; x < params.particle_dims.at(0); ++x) {
        for (size_t y = 0; y < params.particle_dims.at(1); ++y) {
            for (size_t z = 0; z < params.particle_dims.at(2); ++z) {
                std::array pos = {params.anchor_point.at(0) + static_cast<double>(x) * params.spacing,
                                  params.anchor_point.at(1) + static_cast<double>(y) * params.spacing,
                                  params.anchor_point.at(2) + static_cast<double>(z) * params.spacing};

                particles.emplaceBack(pos,
                                      params.velocity + maxwellBoltzmannDistributedVelocity(0.1, third_dim ? 3 : 2),
                                      params.particle_mass, params.type);
            }
        }
    }
    spdlog::debug("Cuboid created at ({}, {}, {}) with dimensions {}x{}x{}, h={}, m={}, type={}",
                  params.anchor_point.at(0), params.anchor_point.at(1), params.anchor_point.at(2),
                  params.particle_dims.at(0), params.particle_dims.at(1), params.particle_dims.at(2), params.spacing,
                  params.particle_mass, params.type);
}

void ParticleGenerator::generateParticle(const ShapeParameters &params, ParticleContainer &particles,
                                         const bool third_dim) {
    particles.emplaceBack(params.anchor_point,
                          params.velocity + maxwellBoltzmannDistributedVelocity(0.1, third_dim ? 3 : 2),
                          params.particle_mass, params.type);
}

void ParticleGenerator::generateEllipsoid(const ShapeParameters &params, ParticleContainer &particles,
                                          const bool third_dim) {
    auto a = static_cast<long long>(params.particle_dims.at(0));
    auto b = static_cast<long long>(params.particle_dims.at(1));
    auto c = static_cast<long long>(params.particle_dims.at(2));

    particles.reserve(particles.size() + static_cast<size_t>(8 * a * b * c));

    for (long long x = 0; x < a; ++x) {
        for (long long y = 0; y < b; ++y) {
            for (long long z = 0; z < c; ++z) {
                if (static_cast<double>(x * x) / static_cast<double>(a * a) +
                        static_cast<double>(y * y) / static_cast<double>(b * b) +
                        static_cast<double>(z * z) / static_cast<double>(c * c) >
                    1) {
                    continue;
                }

                for (int i = 0; i < (third_dim ? 8 : 4); ++i) {
                    const int x_sign = i % 2 < 1 ? -1 : 1;
                    const int y_sign = i % 4 < 2 ? -1 : 1;
                    const int z_sign = i % 8 < 4 ? -1 : 1;

                    std::array<double, 3> pos{};
                    pos.at(0) = params.anchor_point.at(0) + x_sign * (static_cast<double>(x) + 0.5) * params.spacing;
                    pos.at(1) = params.anchor_point.at(1) + y_sign * (static_cast<double>(y) + 0.5) * params.spacing;
                    pos.at(2) =
                        third_dim ? params.anchor_point.at(2) + z_sign * (static_cast<double>(z) + 0.5) * params.spacing
                                  : 0;

                    particles.emplaceBack(pos,
                                          params.velocity + maxwellBoltzmannDistributedVelocity(0.1, third_dim ? 3 : 2),
                                          params.particle_mass, params.type);
                }
            }
        }
    }

    spdlog::debug("Ellipsoid generated at ({}, {}, {}) with dimensions {}x{}x{}, h={}, m={}, type={}",
                  params.anchor_point.at(0), params.anchor_point.at(1), params.anchor_point.at(2), a, b, c,
                  params.spacing, params.particle_mass, params.type);
}
}  // namespace io
