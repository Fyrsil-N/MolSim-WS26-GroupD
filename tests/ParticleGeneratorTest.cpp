//
// Created by hutj on 11.11.26.
//

#include <gtest/gtest.h>

#include <array>
#include <cmath>
#include <cstddef>
#include <set>
#include <string>

#include "ParticleContainer.h"
#include "io/input/ParticleCreator.h"

namespace {
// Same values as the test input file to check against. DONT CHANGE!
const std::string input_file = std::string(TEST_INPUT_DIR) + "/ParticleGeneratorTest_cuboid";
constexpr std::array<double, 3> anchor = {1.0, -2.0, 0.5};
constexpr std::array<int, 3> dims = {3, 2, 4};
constexpr std::array<double, 3> base_velocity = {1.5, -0.5, 2.0};
constexpr double spacing = 1.1225;
constexpr double mass = 2.5;
constexpr int type = 3;
constexpr std::size_t particle_count = 3 * 2 * 4;

ParticleContainer createCuboid() {
    ParticleContainer container;
    io::ParticleCreator(input_file).readInParticles(container);
    return container;
}
}  // namespace

// Generate a cuboid according to a simple test input file and check that the created particle container matches the
// specifications. In particular we check: every point in the cuboid should have a paricle of the correct mass and type.
// This implicitly checks: anchor point, spacing, particle count and dimensions.
TEST(ParticleGeneratorTest, cuboidContainsAllParticles) {
    const ParticleContainer container = createCuboid();
    ASSERT_EQ(container.size(), particle_count);

    // set to store all unique indices visited to check if every grid point has a particle
    std::set<std::array<int, 3>> grid_points;

    for (const auto &p : container) {
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
