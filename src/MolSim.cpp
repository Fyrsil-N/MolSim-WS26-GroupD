
#include <cstdlib>
#include <iostream>
#include <memory>

#include "ParticleContainer.h"
#include "Simulator.h"
#include "cli/CommandLineInterface.h"
#include "forces/Gravity.h"
#include "forces/LennardJones.h"
#include "forces/PairwiseForce.h"
#include "io/input/FileReader.h"
#include "io/input/ParticleCreator.h"
#include "io/output/VTKWriter.h"
#include "io/output/XYZWriter.h"

int main(int argc, char *argv[]) {
    // parse the command line arguments
    auto params = cli::parseArguments(argc, argv);
    if (!params) {
        return EXIT_FAILURE;
    }

    // create the particle container and fill it
    ParticleContainer particles{};
    // io::FileReader file_reader(params->input_file);
    // file_reader.readInParticles(particles);
    // Permanently use the new ParticleCreator instead of the old FileReader
    io::ParticleCreator file_reader(params->input_file);
    file_reader.readInParticles(particles);
    // set up the output methods
    std::vector<std::unique_ptr<io::OutputWriter>> output_writers;
    if (params->vtk_output) {
#ifdef ENABLE_VTK_OUTPUT
        std::string out_name("MD_vtk");
        output_writers.push_back(std::make_unique<io::VTKWriter>(params->output_interval, out_name));
#else  // safe guard should never be reached
        std::cerr << "Invalid Flag -v set. VTK is not available to this build.\n";
        return EXIT_FAILURE;
#endif
    } else {
        std::string out_name("MD_XYZ");
        output_writers.push_back(std::make_unique<io::XYZWriter>(params->output_interval, out_name));
    }

    // instantiate (and combine if necessary) the forces to be used in the simulation
    std::unique_ptr<forces::PairwiseForce> force = std::make_unique<forces::LennardJones>();

    // instantiate and start the Simulator
    Simulator simulator{particles, std::move(force), params->delta_t, params->t_end, std::move(output_writers)};
    std::cout << "Hello from MolSim for PSE!" << '\n' << "Simulating ... " << "\n";
    simulator.runSimulation();
    std::cout << "Simulation complete. Terminating..." << '\n';
    return EXIT_SUCCESS;
}
