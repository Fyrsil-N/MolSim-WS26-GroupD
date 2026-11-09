//
// Created by hutj on 08.11.26.
//

#include <gtest/gtest.h>

#include <array>
#include <cmath>
#include <cstddef>

#include "Particle.h"
#include "forces/LennardJones.h"
#include "utils/ArrayUtils.h"

// Test that distance calculation between particles is always positive
TEST(ParticleTest, distanceCalculation) {
    constexpr std::array<double, 3> x1 = {0, 0, 0};
    constexpr std::array<double, 3> x2 = {-1, -1, -1};

    auto p1 = Particle();
    auto p2 = Particle();

    p1.setX(x1);
    p2.setX(x2);

    ASSERT_EQ(p1.getX(), x1);
    ASSERT_EQ(p2.getX(), x2);

    const auto distance1 = p1.distanceTo(p2);
    const auto distance2 = p2.distanceTo(p1);

    EXPECT_DOUBLE_EQ(distance1, distance2);
    EXPECT_GE(distance1, 0);
    EXPECT_GE(distance2, 0);
    EXPECT_DOUBLE_EQ(distance1, ArrayUtils::L2Norm(x2));
}

// Test that the distance between particles in the same spot is a numerical 0, so a close to zero but not exactly
TEST(ParticleTest, distanceCalculation2) {
    auto p1 = Particle();
    auto p2 = Particle();

    ASSERT_EQ(p1.getX(), p2.getX());

    const auto distance1 = p1.distanceTo(p2);
    const auto distance2 = p2.distanceTo(p1);

    EXPECT_DOUBLE_EQ(distance1, distance2);
    EXPECT_NE(distance1, 0.0);
    EXPECT_NE(distance2, 0.0);

    EXPECT_DOUBLE_EQ(distance1, 0.0);
    EXPECT_DOUBLE_EQ(distance2, 0.0);
}

// Test Lennard Jones Force Calculation on exemplary values along the x axis to make sure attraction, stable state and
// repulsion are correct including their direction and antisymmetry for both particles
TEST(LennardJonesTest, calculateForce) {
    constexpr std::array<double, 3> x1 = {0, 0, 0};

    auto p1 = Particle();
    auto p2 = Particle();

    auto lennard_jones = forces::LennardJones();

    p1.setX(x1);

    // repulsion at 1.0 (p1 is pushed away form p2), stable at 2^(1/6), attraction at 2.0
    const std::array<double, 3> distances = {1.0, pow(2, (1. / 6.)), 2.0};
    constexpr std::array<double, 3> results = {-120.0, 0.0, 0.908203125};
    for (std::size_t i = 0; i < results.size(); ++i) {
        p2.setX({distances.at(i), 0.0, 0.0});

        const auto force1 = lennard_jones.calculateForce(p1, p2);
        const auto force2 = lennard_jones.calculateForce(p2, p1);

        // test antisymmetry
        for (std::size_t j = 0; j < force1.size(); j++) {
            EXPECT_DOUBLE_EQ(force1.at(j), (-1) * force2.at(j));
        }

        // check the results
        EXPECT_NEAR(force1.at(0), results.at(i), 1e-10);
        EXPECT_NEAR(force1.at(1), 0.0, 1e-10);
        EXPECT_NEAR(force1.at(2), 0.0, 1e-10);
    }
}

// Test that the Lennard Jones force acts along an arbitrary direction not just the x-axis
TEST(LennardJonesTest, calculateForce2) {
    // unit vector for just the direction
    constexpr std::array<double, 3> direction = {2. / 7., 3. / 7., 6. / 7.};
    constexpr std::array<double, 3> x1 = {1.0, -2.0, 0.5};

    auto p1 = Particle();
    auto p2 = Particle();

    p1.setX(x1);
    p2.setX(x1 + direction);

    const auto force1 = forces::LennardJones().calculateForce(p1, p2);
    const auto force2 = forces::LennardJones().calculateForce(p2, p1);

    // test antisymmetry
    for (std::size_t j = 0; j < force1.size(); j++) {
        EXPECT_DOUBLE_EQ(force1.at(j), (-1) * force2.at(j));
    }

    // test result
    for (std::size_t j = 0; j < force1.size(); j++) {
        EXPECT_NEAR(force1.at(j), -120.0 * direction.at(j), 1e-10);
    }
}