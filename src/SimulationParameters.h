//
// Created by hutj on 22.10.26.
//

#pragma once
#include <spdlog/spdlog.h>

#include <array>
#include <string>

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
     * Number of spatial dimensions of the simulation (2 or 3).
     * For 2D all particles lay in the z = 0 plane and no brownian motion is applied so they stay there.
     * The linked cells algorithm also ignored the z dimension and doesn't add halo cells there.
     */
    int dimensions = 2;

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