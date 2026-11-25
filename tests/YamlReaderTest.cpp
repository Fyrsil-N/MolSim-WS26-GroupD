//
// Created by hutj on 11.11.26.
//

#include <gtest/gtest.h>

#include <array>
#include <string>

#include "SimulationParameters.h"
#include "io/input/YamlReader.h"

// KNOWN LIMITATION: we currently only test positive paths. We could test all the failure paths with EXPECT_THROW but
// currently the only way to get the bad inputs is through a lot of incorrect yaml files which i don't like.

const std::string full_input_file = std::string(TEST_INPUT_DIR) + "/YamlReaderFullTest.yml";
const std::string minimum_input_file = std::string(TEST_INPUT_DIR) + "/YamlReaderMinimumTest.yml";

namespace {
// params filled with values != any input values so if the yaml reader doesn't write them we can be sure to catch it.
SimulationParameters makeDummyParams() {
    SimulationParameters params;
    params.output_file_basename = "wrong_name";
    params.output_interval = -1;
    params.delta_t = -1;
    params.t_end = -1;
    params.third_dim = true;
    params.r_cutoff = -4;  // not -1 that is default
    params.domain_anchor = {-1, -1, -1};
    params.domain_dimensions = {-1, -1, -1};
    params.boundary_conditions = {{{OUTFLOW, OUTFLOW}, {OUTFLOW, OUTFLOW}, {REFLECTING, REFLECTING}}};
    params.pair_force = GRAVITY;
    params.sigma = -1;
    params.epsilon = -1;
    return params;
}
}  // namespace

// Test that yaml reader readInSimulation correctly parses the input of a valid Yaml Config file
// (YamlReaderFullTest.yml) in particular we test that: all fields are read and the type conversions work as intended
// using the dummy values make sure that we don't take a default value as correctly set.
TEST(YamlReaderTest, readInSimulationTest) {
    io::YamlReader yaml_reader(full_input_file);
    SimulationParameters params = makeDummyParams();
    params.third_dim = false;  // here we set to the opposite of the input file to catch if it remains default.

    yaml_reader.readInSimulation(params);

    EXPECT_EQ(params.output_file_basename, "FullTest");
    EXPECT_DOUBLE_EQ(params.delta_t, 0.0005);
    EXPECT_DOUBLE_EQ(params.t_end, 1.6);
    EXPECT_EQ(params.output_interval, 1000);
    EXPECT_TRUE(params.third_dim);

    constexpr std::array<double, 3> expected_anchor{0, -500, 0.001};
    for (std::size_t d = 0; d < 3; d++) {
        EXPECT_DOUBLE_EQ(params.domain_anchor[d], expected_anchor[d]);
    }

    constexpr std::array<double, 3> expected_dimension{180, 90, 31};
    for (std::size_t d = 0; d < 3; d++) {
        EXPECT_DOUBLE_EQ(params.domain_dimensions[d], expected_dimension[d]);
    }

    EXPECT_EQ(params.boundary_conditions.at(0), (std::array{REFLECTING, REFLECTING}));
    EXPECT_EQ(params.boundary_conditions.at(1), (std::array{OUTFLOW, REFLECTING}));
    EXPECT_EQ(params.boundary_conditions.at(2), (std::array{OUTFLOW, OUTFLOW}));

    EXPECT_EQ(params.pair_force, LENNARD_JONES);
    EXPECT_DOUBLE_EQ(params.sigma, 1.2);
    EXPECT_DOUBLE_EQ(params.epsilon, 2.5);
    EXPECT_DOUBLE_EQ(params.r_cutoff, 3.0);
}

// Test that the yaml reader sets the right default and doesn't fail when optional values are omitted
// in particular we test with the absolute minimum of needed parameters (YamlReaderMinimumTest.yml)
// the dummy params make sure the defaults are really set by the reader and don't just come from the struct
TEST(YamlReaderTest, readInSimulationOptionalsTest) {
    io::YamlReader reader(minimum_input_file);
    SimulationParameters params = makeDummyParams();

    reader.readInSimulation(params);

    // read from the file
    EXPECT_DOUBLE_EQ(params.delta_t, 0.0005);
    EXPECT_DOUBLE_EQ(params.t_end, 1.6);
    EXPECT_EQ(params.boundary_conditions.at(0), (std::array{REFLECTING, REFLECTING}));
    EXPECT_EQ(params.boundary_conditions.at(1), (std::array{OUTFLOW, REFLECTING}));
    EXPECT_EQ(params.pair_force, LENNARD_JONES);
    EXPECT_DOUBLE_EQ(params.sigma, 1);
    EXPECT_DOUBLE_EQ(params.epsilon, 5);

    // fallback default values
    EXPECT_EQ(params.output_file_basename, "MD_output");
    EXPECT_EQ(params.output_interval, 10);
    EXPECT_FALSE(params.third_dim);
    EXPECT_DOUBLE_EQ(params.r_cutoff, -1);

    // since 2d is the fallback default we expect the anchor and dimnesion z value to be set to a default as well
    constexpr std::array<double, 3> expected_anchor{0, -500, 0};
    for (std::size_t d = 0; d < 3; d++) {
        EXPECT_DOUBLE_EQ(params.domain_anchor[d], expected_anchor[d]);
    }

    constexpr std::array<double, 3> expected_dimension{180, 90, 1};
    for (std::size_t d = 0; d < 3; d++) {
        EXPECT_DOUBLE_EQ(params.domain_dimensions[d], expected_dimension[d]);
    }
}
