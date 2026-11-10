//
// Created by hutj on 09.11.26.
//

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>
#include <memory>
#include <utility>
#include <vector>

#include "ParticleContainer.h"
#include "Simulator.h"

namespace {
using ::testing::_;
using ::testing::Return;

class MockOutputWriter : public io::OutputWriter {
   public:
    MockOutputWriter() : OutputWriter(1) {}

    MOCK_METHOD(void, onStep, (const ParticleContainer &particles, int iteration, double time), (override));
    MOCK_METHOD(void, onEnd, (const ParticleContainer &particles), (override));
};

class MockForce : public forces::PairwiseForce {
   public:
    MockForce() { ON_CALL(*this, calculateForce).WillByDefault(Return(std::array<double, 3>{1.0, 0, 0})); }

    MOCK_METHOD((std::array<double, 3>), calculateForce, (const Particle &p1, const Particle &p2), (const, override));
};

class SimulatorTest : public ::testing::Test {
   protected:
    ParticleContainer container;

    SimulatorTest() {
        for (int i = 0; i < 5; i++) {
            const std::array<double, 3> position = {2.0 * i, 0.0, 0.0};
            const std::array<double, 3> velocity = {0.0, 0.0, 0.0};
            container.emplaceBack(position, velocity, 1.0, i);
        }
    }

    // to stop all the "uninteresting call" warnings
    ::testing::NiceMock<MockForce> *force = nullptr;
    ::testing::NiceMock<MockOutputWriter> *writer = nullptr;

    Simulator getMockedSimulator(double delta_t, double t_end, bool parallel) {
        auto force_mock = std::make_unique<::testing::NiceMock<MockForce>>();
        force = force_mock.get();

        auto writer_mock = std::make_unique<::testing::NiceMock<MockOutputWriter>>();
        writer = writer_mock.get();

        std::vector<std::unique_ptr<io::OutputWriter>> output_writers;
        output_writers.push_back(std::move(writer_mock));

        return Simulator{container, std::move(force_mock), delta_t, t_end, parallel, std::move(output_writers)};
    }
};
}  // namespace

// Test that truncation errors don't unintentionally reduce the number of iterations ran
// before a simple static cast would result in delta_t = 0.1 and t_end = 0.3 being only 3 iterations (0, 0.1, 0.2)
TEST_F(SimulatorTest, numberOfIterations) {
    Simulator simulator = getMockedSimulator(0.1, 0.3, false);
    EXPECT_CALL(*writer, onStep(_, _, _)).Times(4);
    simulator.runSimulation();
}

// Make sure that Writers are called with correct arguments and the correct number of times
// especially test that onEnd is called exactly once
TEST_F(SimulatorTest, outpuWriterCalls) {
    Simulator simulator = getMockedSimulator(1, 10, false);
    {
        ::testing::InSequence sequence;
        for (int iteration = 0; iteration <= 10; ++iteration) {
            EXPECT_CALL(*writer, onStep(_, iteration, iteration));
        }
        EXPECT_CALL(*writer, onEnd(_));
    }
    simulator.runSimulation();
}

// Integration Test: run a small simulation and make sure all particles moved correctly
// We use a constant {1, 0, 0} force so particles have constant acceleration and we can check the final forces,
// velocities and positions (unfortunately nothing intermediate).
// We check that the number of force calculations is correct especially that the initial force calculation is executed.
TEST_F(SimulatorTest, testRun1) {
    constexpr double t_end = 3.0;
    Simulator simulator = getMockedSimulator(1.0, t_end, false);

    // one initial force calculation plus one per iteration (3) * all unique pairs
    const auto number_of_pairs = static_cast<int>(container.size() * (container.size() - 1) / 2);
    EXPECT_CALL(*force, calculateForce).Times(4 * number_of_pairs);
    simulator.runSimulation();

    constexpr std::array<double, 5> expected = {4.0, 2.0, 0.0, -2.0, -4.0};

    std::size_t i = 0;
    for (const auto &p : container) {
        // F = m * a here m = 1 so a = F
        const double a = expected.at(i);
        const double x0 = 2.0 * static_cast<double>(i);

        // forces are constant
        EXPECT_DOUBLE_EQ(p.getF().at(0), a);
        // velocities grow linearly
        EXPECT_DOUBLE_EQ(p.getV().at(0), a * t_end);
        // positions grow quadratically
        EXPECT_DOUBLE_EQ(p.getX().at(0), x0 + 0.5 * a * t_end * t_end);
        i++;
    }
}
