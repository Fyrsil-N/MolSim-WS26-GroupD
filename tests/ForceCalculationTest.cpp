//
// Created by hutj on 08.11.26.
//

#include <gtest/gtest.h>

#include <array>
#include <cmath>
#include <cstddef>

#include "TestHelpers.h"
#include "forces/LennardJones.h"
#include "utils/ArrayUtils.h"

using testHelpers::makeParticles;

// Test Lennard Jones Force with no cutoff Calculation on exemplary values along the x-axis to make sure attraction,
// stable state and repulsion are correct including their direction and antisymmetry for both particles
TEST(LennardJonesTest, calculateForce) {
    auto container = makeParticles({{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}});
    auto p1 = container.getHandle(0);
    auto p2 = container.getHandle(1);

    auto lennard_jones = forces::LennardJones(1, 5);

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

// Test that the Lennard Jones force with no cutoff acts along an arbitrary direction not just the x-axis
TEST(LennardJonesTest, calculateForce2) {
    // unit vector for just the direction
    constexpr std::array<double, 3> direction = {2. / 7., 3. / 7., 6. / 7.};
    constexpr std::array<double, 3> x1 = {1.0, -2.0, 0.5};

    auto container = makeParticles({x1, x1 + direction});
    auto p1 = container.getHandle(0);
    auto p2 = container.getHandle(1);

    const auto force1 = forces::LennardJones(1, 5).calculateForce(p1, p2);
    const auto force2 = forces::LennardJones(1, 5).calculateForce(p2, p1);

    // test antisymmetry
    for (std::size_t j = 0; j < force1.size(); j++) {
        EXPECT_DOUBLE_EQ(force1.at(j), (-1) * force2.at(j));
    }

    // test result
    for (std::size_t j = 0; j < force1.size(); j++) {
        EXPECT_NEAR(force1.at(j), -120.0 * direction.at(j), 1e-10);
    }
}

// Test that setting sigma and epsilon to non default values works and gives correct results.
TEST(LennardJonesTest, calculateForceSigmaEpsilon) {
    auto container = makeParticles({{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}});
    auto p1 = container.getHandle(0);
    auto p2 = container.getHandle(1);

    constexpr double sigma = 2.0;
    constexpr double epsilon = 3.0;
    auto lennard_jones = forces::LennardJones(sigma, epsilon);

    // repulsion at sigma, stable at 2^(1/6) * sigma, attraction at 2 * sigma
    const std::array<double, 3> distances = {sigma, pow(2, (1. / 6.)) * sigma, 2 * sigma};
    constexpr std::array<double, 3> results = {-36.0, 0.0, 0.2724609375};
    for (std::size_t i = 0; i < results.size(); ++i) {
        p2.setX({distances.at(i), 0.0, 0.0});

        const auto force = lennard_jones.calculateForce(p1, p2);

        EXPECT_NEAR(force.at(0), results.at(i), 1e-10);
        EXPECT_NEAR(force.at(1), 0.0, 1e-10);
        EXPECT_NEAR(force.at(2), 0.0, 1e-10);
    }
}

// Test that setting a cutoff with a value greater 0 applies the cutoff radius and a cutoff <= 0 doesn't.
// Test this by checking a particle just beyond cutoff.
TEST(LennardJonesCutOffTest, useCutoff) {
    auto container = makeParticles({{0.0, 0.0, 0.0}, {2.0, 0.0, 0.0}});
    auto p1 = container.getHandle(0);
    auto p2 = container.getHandle(1);

    auto valid_cutoff = forces::LennardJones(1, 5, 1);
    auto cutoff_off = forces::LennardJones(1, 5, -1);
    auto cutoff_zero = forces::LennardJones(1, 5, 0);

    auto force1 = valid_cutoff.calculateForce(p1, p2);
    auto force2 = cutoff_off.calculateForce(p1, p2);
    auto force3 = cutoff_zero.calculateForce(p1, p2);

    // beyond cutoff -> no force
    EXPECT_NEAR(force1.at(0), 0.0, 1e-10);
    EXPECT_NEAR(force1.at(1), 0.0, 1e-10);
    EXPECT_NEAR(force1.at(2), 0.0, 1e-10);

    // no cutoff -> force
    EXPECT_NEAR(force2.at(0), 0.908203125, 1e-10);
    EXPECT_NEAR(force2.at(1), 0.0, 1e-10);
    EXPECT_NEAR(force2.at(2), 0.0, 1e-10);

    // edge case cutoff = 0 should mean no cutoff -> force
    EXPECT_NEAR(force3.at(0), 0.908203125, 1e-10);
    EXPECT_NEAR(force3.at(1), 0.0, 1e-10);
    EXPECT_NEAR(force3.at(2), 0.0, 1e-10);
}

// Test Lennard Jones Force with cutoff Calculation on exemplary values along the x-axis to make sure attraction,
// stable state and repulsion are correct including their direction and antisymmetry for both particles.
// Also ensure that a particle outside the cutoff returns a force of 0.
TEST(LennardJonesCutOffTest, calculateForceWithCutoff) {
    auto container = makeParticles({{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}});
    auto p1 = container.getHandle(0);
    auto p2 = container.getHandle(1);

    auto lennard_jones = forces::LennardJones(1, 5, 3);

    // repulsion at 1.0 (p1 is pushed away form p2), stable at 2^(1/6), attraction at 2.0
    const std::array<double, 4> distances = {1.0, pow(2, (1. / 6.)), 2.0, 4.0};
    constexpr std::array<double, 4> results = {-120.0, 0.0, 0.908203125, 0.0};
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

// Make sure that a particle on the cutoff radius returns a valid force and not 0.
// Make sure this works in an arbitrary direction
TEST(LennardJonesCutOffTest, particleAtCutoff) {
    auto container = makeParticles({{0.0, 0.0, 0.0}, {0.3, -2.0, 1.0}});
    auto p1 = container.getHandle(0);
    auto p2 = container.getHandle(1);

    auto lennard_jones = forces::LennardJones(1, 5, ArrayUtils::L2Norm(p2.getX()));

    auto force = lennard_jones.calculateForce(p1, p2);

    constexpr std::array<double, 3> result = {0.0528195040, -0.3521300267, 0.1760650133};
    for (std::size_t j = 0; j < force.size(); j++) {
        EXPECT_NEAR(force.at(j), result.at(j), 1e-10);
    }
}