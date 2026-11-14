//
// Created by hutj on 22.10.26.
//

#pragma once
#include <spdlog/spdlog.h>

#include <string>

/**
 * Supported formats of input files
 */
enum class FileFormat {
    /**
     * List of single particles with position, velocity and mass for the io::FileReader
     */
    raw,
    /**
     * Definitions of shapes like cuboids filled with particles for the io::ParticleCreator
     */
    generator,
};

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
     * The program generates an output state every output_interval iterations
     */
    int output_interval = 10;

    /**
     * VTK output flag (xyz if false)
     */
    bool vtk_output = false;

    /**
     * Format of the input file
     */
    FileFormat file_format = FileFormat::generator;

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