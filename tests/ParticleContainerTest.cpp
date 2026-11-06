//
// Created by hutj on 02.11.26.
//

#include <gtest/gtest.h>

#include <set>
#include <utility>

#include "ParticleContainer.h"

namespace {
class TestParticleContainer : public ::testing::Test {
   protected:
    ParticleContainer container;

    TestParticleContainer() {
        // fill the container with 10 particles indexed using their type
        for (int i = 0; i < 10; i++) {
            container.emplaceBack(i);
        }
    }
};
}  // namespace

// Test Constructing a ParticleContainer and filling it with emplace back.
TEST_F(TestParticleContainer, constructorTest) {
    ASSERT_EQ(container.size(), 10);

    auto particle_iter = container.begin();
    for (int i = 0; i < 10; i++) {
        EXPECT_EQ(particle_iter->getType(), i);
        ++particle_iter;
    }
}

// Test that filling a particle container after specifying the required size with reserve does not need to copy or
// relocate anything.
TEST_F(TestParticleContainer, reserveTest) {
    ASSERT_EQ(container.size(), 10);

    // reserve space for 10 additional particles
    container.reserve(container.size() + 10);

    // remember the position of the first element in memory
    const Particle *first_particle_ptr = std::to_address(container.begin());

    // fill the container with 10 particles indexed using their type
    for (int i = 10; i < 20; i++) {
        container.emplaceBack(i);
    }

    ASSERT_EQ(container.size(), 20);
    // make sure the first element (and by extension the entire vector) has not moved
    ASSERT_EQ(first_particle_ptr, std::to_address(container.begin()));

    auto particle_iter = container.begin();
    for (int i = 0; i < 20; i++) {
        EXPECT_EQ(particle_iter->getType(), i);
        ++particle_iter;
    }
}

// Test that the ParticleContainer::forEachPair actually goes over each unique pair exactly once.
TEST_F(TestParticleContainer, forEachPairTest) {
    int function_calls = 0;
    std::set<std::pair<int, int>> pairs;

    // compute the number of unique pairs
    std::size_t expected_number_of_pairs = container.size() * (container.size() - 1) / 2;

    auto helper = [&function_calls, &pairs](Particle &p1, Particle &p2) -> std::array<double, 3> {
        // #pragma omp critical
        {
            function_calls++;
            // make sure we never call on equal particles
            EXPECT_NE(&p1, &p2);
            // remember each pair we visited (no duplicates)
            if (p1.getType() < p2.getType()) {
                pairs.emplace(p1.getType(), p2.getType());
            } else {
                pairs.emplace(p2.getType(), p1.getType());
            }
            return {0.0, 0.0, 0.0};
        }
    };
    container.forEachPair(helper, false);

    ASSERT_EQ(function_calls, expected_number_of_pairs);
    ASSERT_EQ(pairs.size(), expected_number_of_pairs);
}