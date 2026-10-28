//
// Created by hutj on 22.10.26.
//

#pragma once
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
     * The program generates an output state every output_interval iterations
     */
    int output_interval = 10;

    /**
     * VTK output flag (xyz if false)
     */
    bool vtk_output = false;
};