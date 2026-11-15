//
// Created by hutj on 02.11.26.
//

#include <gtest/gtest.h>

#include <array>
#include <cstddef>
#include <set>
#include <utility>

#include "ParticleContainer/ConstParticleHandle.h"
#include "ParticleContainer/ParticleContainerDirectSum.h"
#include "forces/PairwiseForce.h"

namespace {
// Expose the memory address of the data so that we can check for relocations later on
class InspectableContainer : public ParticleContainerDirectSum {
   public:
    const void *positionsAddress() const { return data.positions.data(); }
};

class ParticleContainerTest : public ::testing::Test {
   protected:
    InspectableContainer container;

    ParticleContainerTest() {
        // fill the container with 10 particles indexed using their type
        for (int i = 0; i < 10; i++) {
            container.emplaceBack({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, 1.0, i);
        }
    }
};

// Force that counts its calls and records every pair it is called with replaces the old force lambda
class CallCountingForce : public forces::PairwiseForce {
   public:
    mutable int function_calls = 0;
    mutable std::set<std::pair<int, int>> pairs;

    std::array<double, 3> calculateForce(const ConstParticleHandle p1, const ConstParticleHandle p2) const override {
        function_calls++;
        // make sure we never call on equal particles
        EXPECT_NE(p1.getType(), p2.getType());
        // remember each pair we visited (no duplicates)
        if (p1.getType() < p2.getType()) {
            pairs.emplace(p1.getType(), p2.getType());
        } else {
            pairs.emplace(p2.getType(), p1.getType());
        }
        return {0.0, 0.0, 0.0};
    }
};

// Force that is different for every pair, so that a missing or duplicated pair changes the result
class UniquePairsForce : public forces::PairwiseForce {
   public:
    std::array<double, 3> calculateForce(const ConstParticleHandle p1, const ConstParticleHandle p2) const override {
        const double value = (p1.getType() + 1.0) * (p2.getType() + 1.0);
        return {-value, value, 2 * value};
    }
};
}  // namespace

// Test Constructing a ParticleContainer and filling it with emplace back.
TEST_F(ParticleContainerTest, constructor) {
    ASSERT_EQ(container.size(), 10);

    for (std::size_t i = 0; i < 10; i++) {
        EXPECT_EQ(container.getConstHandle(i).getType(), i);
    }
}

// Test that filling a particle container after specifying the required size with reserve does not need to copy or
// relocate anything.
TEST_F(ParticleContainerTest, reserve) {
    ASSERT_EQ(container.size(), 10);

    // reserve space for 10 additional particles
    container.reserve(container.size() + 10);

    // remember the position of the particle data in memory
    const void *positions_address = container.positionsAddress();

    // fill the container with 10 particles indexed using their type
    for (int i = 10; i < 20; i++) {
        container.emplaceBack({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, 1.0, i);
    }

    ASSERT_EQ(container.size(), 20);
    // make sure the data (and by extension all particles) has not moved
    ASSERT_EQ(positions_address, container.positionsAddress());

    for (std::size_t i = 0; i < 20; i++) {
        EXPECT_EQ(container.getConstHandle(i).getType(), i);
    }
}

// Test that ParticleContainerDirectSum::updateForce actually goes over each unique pair exactly once
TEST_F(ParticleContainerTest, updateForce) {
    CallCountingForce force;

    // compute the number of unique pairs
    std::size_t expected_number_of_pairs = container.size() * (container.size() - 1) / 2;

    container.updateForce(force);

    ASSERT_EQ(force.function_calls, expected_number_of_pairs);
    ASSERT_EQ(force.pairs.size(), expected_number_of_pairs);
}

// Test that updateForce correctly adds the force to one and subtracts it from the other particle (newton's third law)
TEST(ParticleContainerForceTest, updateForceAccumulatesForces) {
    const UniquePairsForce pair_force;

    // pair (0, 1) -> 2, pair (0, 2) -> 3, pair (1, 2) -> 6
    constexpr std::array<double, 3> expected = {2.0 + 3.0, -2.0 + 6.0, -3.0 - 6.0};

    ParticleContainerDirectSum container;
    for (int i = 0; i < 3; i++) {
        container.emplaceBack({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, 1.0, i);
    }

    container.updateForce(pair_force);

    std::size_t index = 0;
    for (const auto &p : container) {
        EXPECT_DOUBLE_EQ(p.getF().at(0), -expected.at(index));
        EXPECT_DOUBLE_EQ(p.getF().at(1), expected.at(index));
        EXPECT_DOUBLE_EQ(p.getF().at(2), 2 * expected.at(index));
        ++index;
    }
}
