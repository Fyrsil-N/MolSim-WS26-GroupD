
#include <cstdlib>
#include <iostream>

#include "ParticleContainer.h"
#include "cli/CommandLineInterface.h"
#include "io/input/FileReader.h"
#include "io/output/VTKWriter.h"
#include "io/output/XYZWriter.h"
#include "utils/ArrayUtils.h"

/**** forward declaration of the calculation functions ****/

/**
 * calculate the force for all particles
 */
void calculateF();

/**
 * calculate the position for all particles
 */
void calculateX(double delta_t);

/**
 * calculate the velocity for all particles
 */
void calculateV(double delta_t);

/**
 * plot the particles to a xyz-file or a vtk file if the cmake flag is set
 */
void plotParticles(int iteration);

ParticleContainer particles{};

int main(int argc, char *argv[]) {
  std::cout << "Hello from MolSim for PSE!" << '\n';

  auto params = cli::parseArguments(argc, argv);
  if (!params) {
    return EXIT_FAILURE;
  }

  io::FileReader file_reader;
  file_reader.readFile(particles, params->input_file.data());

  double current_time = 0;

  int iteration = 0;

  // for this loop, we assume: current x, current f and current v are known
  while (current_time < params->t_end) {
    // calculate new x
    calculateX(params->delta_t);
    // calculate new f
    calculateF();
    // calculate new v
    calculateV(params->delta_t);

    iteration++;
    if (iteration % params->output_interval == 0) {
      plotParticles(iteration);
    }
    std::cout << "Iteration " << iteration << " finished." << '\n';

    current_time += params->delta_t;
  }

  std::cout << "output written. Terminating..." << '\n';
  return EXIT_SUCCESS;
}

// @TODO here we can make use of the pairwise iteration later
void calculateF() {
  for (auto &p1 : particles) {
    std::array<double, 3> force_i{};
    for (auto &p2 : particles) {
      if (p1 == p2) {
        continue;
      }
      const auto diff = p2.getX() - p1.getX();
      const double dist = ArrayUtils::L2Norm(diff);
      force_i = force_i + (p1.getM() * p2.getM() / (dist * dist * dist)) * diff;
    }
    p1.updateF(force_i);
  }
}

void calculateX(double delta_t) {
  for (auto &p : particles) {
    auto delta_x = delta_t * p.getV() + delta_t * delta_t / (2 * p.getM()) * p.getF();
    p.setX(p.getX() + delta_x);
  }
}

void calculateV(double delta_t) {
  for (auto &p : particles) {
    auto delta_v = delta_t / (2 * p.getM()) * (p.getOldF() + p.getF());
    p.setV(p.getV() + delta_v);
  }
}

void plotParticles(int iteration) {
  std::string out_name("MD_vtk");

#ifdef ENABLE_VTK_OUTPUT
  io::VTKWriter writer;
#else
  io::XYZWriter writer;
#endif
  writer.plotParticles(particles, out_name, iteration);
}
