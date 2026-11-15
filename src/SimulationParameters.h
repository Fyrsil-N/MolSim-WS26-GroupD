//
// Created by hutj on 22.10.26.
//

#pragma once
#include <spdlog/spdlog.h>

#include <array>
#include <string>

#include "forces/LennardJones.h"

/**
 * Parameters for a simulation run
 */
struct SimulationParameters {
    /**
     * Path to the input_file with initial particle settings
     */
    std::string input_file;

    /**
     * Time step size of the simulation
     */
    double delta_t = 0.014;

    /**
     * End time of the simulation
     */
    double t_end = 1000;

    /**
     * Whether the third spatial dimension is enabled
     */
    bool third_dim = false;

    /**
     * Cut off radius for the linked cell algorithm
     */
    double r_cutoff = 3.0;

    /**
     * The simulation domain starts at these (x, y, z) coordinates.
     */
    std::array<double, 3> domain_anchor = {0, 0, -3};

    /**
     * Dimensions of the simulation Domain in (x,y,z) directions starting from the domain_anchor
     * Includes both inner and boundary cells not halo cells.
     */
    std::array<double, 3> domain_dimensions = {180, 90, 6};

    /**
     * The program generates an output state every output_interval iterations
     */
    int output_interval = 10;

    /**
     * The force to be applied to every pair of particles
     */
    std::unique_ptr<forces::PairwiseForce> pair_force = std::make_unique<forces::LennardJones>();

    /**
     * VTK output flag (xyz if false)
     */
    bool vtk_output = false;

    /**
     * OpenMP activation flag (parallel execution if true)
     */
#ifdef ENABLE_OPENMP
    bool omp_enabled = true;
#else
    bool omp_enabled = false;
#endif

    /**
     * log level for the execution (trace, debug, info, warn, err, critical, off)
     */
    spdlog::level::level_enum log_level = spdlog::level::info;

    /**
     * Flag to activate a benchmark run (time the simulation using the PerformanceWriter)
     */
    bool benchmark_run = false;
};