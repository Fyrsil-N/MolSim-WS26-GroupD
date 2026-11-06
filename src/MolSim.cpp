#include <spdlog/spdlog.h>

#include <filesystem>

#include "ParticleContainer.h"
#include "Simulator.h"
#include "cli/CommandLineInterface.h"
#include "forces/LennardJones.h"
#include "forces/PairwiseForce.h"
#include "io/input/ParticleCreator.h"
#include "io/output/VTKWriter.h"
#include "io/output/XYZWriter.h"

int main(int argc, char *argv[]) {
    // parse the command line arguments
    auto params = cli::parseArguments(argc, argv);
    if (!params) {
        return EXIT_FAILURE;
    }

    // set the log level
    spdlog::set_level(params->log_level);

    // create the particle container and fill it
    ParticleContainer particles{};

    // use the new ParticleCreator instead of the old FileReader
    io::ParticleCreator file_reader(params->input_file);
    file_reader.readInParticles(particles);

    // set up the output methods
    std::vector<std::unique_ptr<io::OutputWriter>> output_writers;

    std::filesystem::path out_dir("./out_0");
    for (int i = 0; std::filesystem::exists(out_dir); ++i) {
        out_dir = std::filesystem::path("./out_" + std::to_string(i));
        spdlog::info("{0:d}", i);
    }

    std::filesystem::create_directory(out_dir);
    spdlog::info("Outputting to {}.", out_dir.string());

    if (params->vtk_output) {
#ifdef ENABLE_VTK_OUTPUT
        std::string out_name(out_dir.string() + "/MD_vtk");
        output_writers.push_back(std::make_unique<io::VTKWriter>(params->output_interval, out_name));
#else  // safe guard, should never be reached (cli::parseArguments already rejects -v)
        spdlog::error("invalid flag -v set: VTK output is not available in this build");
        return EXIT_FAILURE;
#endif
    } else {
        std::string out_name(out_dir.string() + "/MD_XYZ");
        output_writers.push_back(std::make_unique<io::XYZWriter>(params->output_interval, out_name));
    }

    // instantiate (and combine if necessary) the forces to be used in the simulation
    std::unique_ptr<forces::PairwiseForce> force = std::make_unique<forces::LennardJones>();

    // instantiate and start the Simulator
    Simulator simulator{particles, std::move(force), params->delta_t, params->t_end, std::move(output_writers)};
    spdlog::info("Hello from MolSim for PSE! Simulating ...");
    simulator.runSimulation();
    spdlog::info("Simulation complete. Terminating...");
    return EXIT_SUCCESS;
}
