//
// Created by hutj on 25.11.26.
//

#include <gtest/gtest.h>

#include <array>
#include <cmath>
#include <set>

#include "ParticleContainer/DirectSumContainer.h"
#include "io/input/ParticleGenerator.h"
#include "io/input/ShapeParameters.h"

constexpr std::array<double, 3> anchor = {1.0, -2.0, 0.5};
constexpr std::array<double, 3> anchor2d = {anchor[0], anchor[1], 0};
constexpr std::array<size_t, 3> dims = {3, 2, 4};
constexpr std::array<size_t, 3> dims2d = {dims[0], dims[1], 1};
constexpr std::array<size_t, 3> ellipsoid_dims2d = {4, 3, 1};
constexpr std::array<double, 3> velocity = {3, 2, 4};
constexpr std::array<double, 3> velocity2d = {velocity[0], velocity[1], 0};
constexpr double spacing = 1.1225;
constexpr double mass = 2.5;
constexpr int type = 3;

// Create a single particle using the particle creator and check if every value is set correctly
TEST(ParticleGeneratorTest, generateParticle3DTest) {
    io::ParticleGenerator generator{};
    const std::unique_ptr<ParticleContainer> container_ptr = std::make_unique<DirectSumContainer>();

    // spacing and dimension shouldn't matter and are left initialized for that reason
    constexpr ShapeParameters cuboid_params = {anchor, {}, velocity, {}, mass, type};

    generator.generateParticle(cuboid_params, *container_ptr, true);
    ASSERT_EQ(container_ptr->size(), 1);

    // loop over x, y, z coordinate
    auto p = container_ptr->getHandle(0);

    for (int d = 0; d < 3; d++) {
        EXPECT_NEAR(p.getX()[d], anchor[d], 1e-9);
    }

    EXPECT_DOUBLE_EQ(p.getM(), mass);
    EXPECT_EQ(p.getType(), type);
}

// Generate a cuboid according to a simple test input and check that the created particle container matches the
// specifications. In particular, we check: every point in the cuboid should have a particle of the correct mass and
// type. This implicitly checks: anchor point, spacing, particle count and dimensions.
TEST(ParticleGeneratorTest, generateCuboid3DTest) {
    io::ParticleGenerator generator{};
    const std::unique_ptr<ParticleContainer> container_ptr = std::make_unique<DirectSumContainer>();

    constexpr ShapeParameters cuboid_params = {anchor, dims, velocity, spacing, mass, type};
    constexpr std::size_t particle_count = dims[0] * dims[1] * dims[2];

    generator.generateCuboid(cuboid_params, *container_ptr, true);

    ASSERT_EQ(container_ptr->size(), particle_count);

    // set to store all unique indices visited to check if every grid point has a particle
    std::set<std::array<int, 3>> grid_points;

    for (const auto &p : *container_ptr) {
        std::array<int, 3> index{};
        // loop over x, y, z coordinate
        for (int d = 0; d < 3; d++) {
            // calculate the offset to the anchor point (so basically the index of the particle in the cuboid)
            const double k = (p.getX().at(d) - anchor.at(d)) / spacing;

            index.at(d) = static_cast<int>(std::lround(k));

            EXPECT_NEAR(k, index.at(d), 1e-9) << "particle is not on the grid";
            EXPECT_GE(index.at(d), 0);
            EXPECT_LT(index.at(d), dims.at(d));
        }
        grid_points.emplace(index);

        EXPECT_DOUBLE_EQ(p.getM(), mass);
        EXPECT_EQ(p.getType(), type);
    }

    EXPECT_EQ(grid_points.size(), particle_count);
}

// Generate a cuboid in 2d dimensions and make sure that the brownian motion is only applied to x and y not z
// Also performs the same tests as the generateCuboid3DTest
TEST(ParticleGeneratorTest, generateCuboid2DTest) {
    io::ParticleGenerator generator{};
    const std::unique_ptr<ParticleContainer> container_ptr = std::make_unique<DirectSumContainer>();

    constexpr ShapeParameters cuboid_params = {anchor, dims2d, velocity2d, spacing, mass, type};
    constexpr size_t particle_count = dims[0] * dims[1];

    generator.generateCuboid(cuboid_params, *container_ptr, false);

    ASSERT_EQ(container_ptr->size(), particle_count);

    // set to store all unique indices visited to check if every grid point has a particle
    std::set<std::array<int, 3>> grid_points;

    for (const auto &p : *container_ptr) {
        std::array<int, 3> index{};
        // loop over x, y, z coordinate
        for (int d = 0; d < 3; d++) {
            // calculate the offset to the anchor point (so basically the index of the particle in the cuboid)
            const double k = (p.getX().at(d) - anchor.at(d)) / spacing;

            index.at(d) = static_cast<int>(std::lround(k));

            EXPECT_NEAR(k, index.at(d), 1e-9) << "particle is not on the grid";
            EXPECT_GE(index.at(d), 0);
            EXPECT_LT(index.at(d), dims.at(d));
        }
        grid_points.emplace(index);

        EXPECT_DOUBLE_EQ(p.getM(), mass);
        EXPECT_EQ(p.getType(), type);
        // no velocity in z direction
        EXPECT_NEAR(p.getV()[2], 0.0, 1e-10);
    }

    EXPECT_EQ(grid_points.size(), particle_count);
}

// Generate a 2D ellipse (anchor = center, axis a = 4, b = 3) and check that the particles lay within the ellipsoid.
// a point is within the ellipsoid if x^2/a^2 + y^2/b^2 <= 1 which is what we check.
// we also check that particles lay on the grid so spacing, and normal things like mass and type and velocity in z being
// 0.
TEST(ParticleGeneratorTest, generateEllipsoid2DTest) {
    io::ParticleGenerator generator{};
    const std::unique_ptr<ParticleContainer> container_ptr = std::make_unique<DirectSumContainer>();

    constexpr ShapeParameters ellipsoid_params = {anchor2d, ellipsoid_dims2d, velocity2d, spacing, mass, type};
    constexpr size_t particle_count = 4 * 11;

    generator.generateEllipsoid(ellipsoid_params, *container_ptr, false);

    ASSERT_EQ(container_ptr->size(), particle_count);

    std::set<std::array<int, 2>> grid_points;

    for (const auto &p : *container_ptr) {
        std::array<int, 2> index{};
        double ellipse_value = 0;
        // loop over x, y coordinate
        for (int d = 0; d < 2; d++) {
            // offset to the center of the ellipsoid
            const double k = (p.getX().at(d) - anchor2d.at(d)) / spacing;

            index.at(d) = static_cast<int>(std::lround(k - 0.5));
            EXPECT_NEAR(k, index.at(d) + 0.5, 1e-9) << "particle is not on the grid";

            // index is relative to the anchor and can be negative but the grid is shifted so we need the -1
            // kind of ugly but essentially -1 corresponds to 0, -2 to 1 when looking at the axis symmetries
            const int i = index.at(d) >= 0 ? index.at(d) : -index.at(d) - 1;
            EXPECT_LT(i, ellipsoid_dims2d.at(d));

            const auto axis = static_cast<double>(ellipsoid_dims2d.at(d));
            ellipse_value += (i * i) / (axis * axis);
        }
        EXPECT_LE(ellipse_value, 1.0) << "particle is outside the ellipse";
        grid_points.emplace(index);

        EXPECT_DOUBLE_EQ(p.getX()[2], 0.0);
        EXPECT_DOUBLE_EQ(p.getM(), mass);
        EXPECT_EQ(p.getType(), type);
        // no velocity in z direction
        EXPECT_NEAR(p.getV()[2], 0.0, 1e-10);
    }

    EXPECT_EQ(grid_points.size(), particle_count);
}