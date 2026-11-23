//
// Created by hutj on 22.10.26.
//

#pragma once

#include <spdlog/spdlog.h>

#include <array>
#include <string>

/**
 * enum for the different boundary conditions supported.
 * Currently only outflow or reflecting boundaries, later on also periodic
 */
enum BoundaryCondition { OUTFLOW, REFLECTING };

/**
 * enum for the different kinds of forces that can be used in the simulation.
 */
enum Forces { LENNARD_JONES, GRAVITY };

/**
 * Parameters for a simulation run
 */
struct SimulationParameters {
    /**
     * Path to the input_file with initial particle settings
     */
    std::string input_file;

    /**
     * Path to the input_file with initial particle settings
     */
    /**
     * Base name of the output file (if an output file is used). The basename is prefixed to all files generated.
     * eg. <output_file_basename>_0003.vtu
     */
    std::string output_file_basename = "MD_output";

    /**
     * VTK output flag (xyz if false)
     */
    bool vtk_output = false;

    /**
     * The program generates an output state every output_interval iterations
     */
    int output_interval = 10;

    /**
     * log level for the execution (trace, debug, info, warn, err, critical, off)
     */
    spdlog::level::level_enum log_level = spdlog::level::info;

    /**
     * OpenMP activation flag (parallel execution if true)
     */
#ifdef ENABLE_OPENMP
    bool omp_enabled = true;
#else
    bool omp_enabled = false;
#endif

    /**
     * Flag to activate a benchmark run (time the simulation using the PerformanceWriter)
     */
    bool benchmark_run = false;

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
     * Cut off radius for the linked cell algorithm.
     * Negative values mean we don't make use of the cutoff or linked cell implementation.
     */
    double r_cutoff = -1;

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
     * Boundary condition for each spatial boundary.
     * Index 0 and 1 are for the x dimension
     * Index 2 and 3 are for the y dimension
     * Index 4 and 5 are for the z dimension and remain unused if third_dim is false
     */
    std::array<BoundaryCondition, 6> boundary_conditions{};

    /**
     * force that should be applied to two particles in the simulation
     */
    Forces pair_force = LENNARD_JONES;

    /**
     * sigma parameter for the lennard jones force
     */
    double sigma = 1;

    /**
     * epsilon parameter for the lennard jones force
     */
    double epsilon = 3;
};