#include <spdlog/spdlog.h>

#include <filesystem>

#include "ParticleContainer.h"
#include "Simulator.h"
#include "cli/CommandLineInterface.h"
#include "forces/LennardJones.h"
#include "forces/PairwiseForce.h"
#include "io/input/ParticleCreator.h"
#include "io/output/PerformanceWriter.h"
#include "io/output/ProgressWriter.h"
#include "io/output/VTKWriter.h"
#include "io/output/XYZWriter.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace {
/**
 * Setup the Required OutputWriters
 * @param params Simulation parameters which dictate which writers are used in the run
 * @return Vector of all OutputWriters to be used in the simulation
 */
std::vector<std::unique_ptr<io::OutputWriter>> setupOutputWriters(const SimulationParameters &params) {
    std::vector<std::unique_ptr<io::OutputWriter>> output_writers;

    // benchmark runs only allow for the PerformanceWriter
    if (params.benchmark_run) {
        output_writers.push_back(std::make_unique<io::PerformanceWriter>(params.omp_enabled));
        return output_writers;
    }

    output_writers.push_back(std::make_unique<io::ProgressWriter>(static_cast<int>(params.t_end / params.delta_t)));

    if (!std::filesystem::exists("../output")) {
        std::filesystem::create_directory("../output");
    }

    std::filesystem::path out_dir("../output/out_0");
    for (int i = 0; std::filesystem::exists(out_dir); ++i) {
        out_dir = std::filesystem::path("../output/out_" + std::to_string(i));
    }

    std::filesystem::create_directory(out_dir);
    spdlog::info("Outputting to {}.", out_dir.string());

    if (params.vtk_output) {
#ifdef ENABLE_VTK_OUTPUT
        std::string out_name(out_dir.string() + "/MD_vtk");
        output_writers.push_back(std::make_unique<io::VTKWriter>(params.output_interval, out_name));
#else  // safe guard, should never be reached (cli::parseArguments already rejects -v)
        spdlog::error("invalid flag -v set: VTK output is not available in this build");
        std::exit(EXIT_FAILURE);
#endif
    } else {
        std::string out_name(out_dir.string() + "/MD_XYZ");
        output_writers.push_back(std::make_unique<io::XYZWriter>(params.output_interval, out_name));
    }

    return output_writers;
}
}  // namespace

int main(int argc, char *argv[]) {
    spdlog::set_default_logger(spdlog::stderr_color_mt("molsim"));

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
    std::vector<std::unique_ptr<io::OutputWriter>> output_writers = setupOutputWriters(params.value());

    // instantiate (and combine if necessary) the forces to be used in the simulation
    std::unique_ptr<forces::PairwiseForce> force = std::make_unique<forces::LennardJones>();

    // disable in-simulation logging for benchmarks
    if (params->benchmark_run) {
        spdlog::warn("Running benchmark and disabling all following logs...");
        spdlog::set_level(spdlog::level::off);
    }

    // instantiate and start the Simulator
    Simulator simulator{particles,     std::move(force),    params->delta_t,
                        params->t_end, params->omp_enabled, std::move(output_writers)};
    spdlog::info("Hello from MolSim for PSE! Simulating for {0:d} iterations...\n",
                 static_cast<int>(params->t_end / params->delta_t));
    simulator.runSimulation();
    spdlog::info("Simulation complete. Terminating...");
    return EXIT_SUCCESS;
}
