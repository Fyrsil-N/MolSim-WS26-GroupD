
#include <iostream>
#include <list>

#include "FileReader.h"
#include "outputWriter/VTKWriter.h"
#include "outputWriter/XYZWriter.h"
#include "utils/ArrayUtils.h"

/**** forward declaration of the calculation functions ****/

/**
 * calculate the force for all particles
 */
void calculateF();

/**
 * calculate the position for all particles
 */
void calculateX();

/**
 * calculate the position for all particles
 */
void calculateV();

/**
 * plot the particles to a xyz-file
 */
void plotParticles(int iteration);

constexpr double start_time = 0;
constexpr double end_time = 1000;
constexpr double delta_t = 0.014;

// TODO: what data structure to pick?
std::list<Particle> particles;

// TODO: get some proper parameter passing and validation going and also add a help message
int main(int argc, char *argsv[]) {
  std::cout << "Hello from MolSim for PSE!" << '\n';
  if (argc != 2) {
    std::cout << "Erroneous programme call! " << '\n';
    std::cout << "./molsym filename" << '\n';
  }

  FileReader file_reader;
  file_reader.readFile(particles, argsv[1]);

  double current_time = start_time;

  int iteration = 0;

  // for this loop, we assume: current x, current f and current v are known
  while (current_time < end_time) {
    // calculate new x
    calculateX();
    // calculate new f
    calculateF();
    // calculate new v
    calculateV();

    iteration++;
    if (iteration % 10 == 0) {
      plotParticles(iteration);
    }
    std::cout << "Iteration " << iteration << " finished." << '\n';

    current_time += delta_t;
  }

  std::cout << "output written. Terminating..." << '\n';
  return 0;
}

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

void calculateX() {
  for (auto &p : particles) {
    auto delta_x = delta_t * p.getV() + delta_t * delta_t / (2 * p.getM()) * p.getOldF();
    p.setX(p.getX() + delta_x);
  }
}

void calculateV() {
  for (auto &p : particles) {
    auto delta_v = delta_t / (2 * p.getM()) * (p.getOldF() + p.getF());
    p.setV(p.getV() + delta_v);
  }
}

// TODO change the vtk::plotparticle to a pass by reference
void plotParticles(int iteration) {
  std::string out_name("MD_vtk");

#ifdef ENABLE_VTK_OUTPUT
  outputWriter::VTKWriter writer;
#else
  outputWriter::XYZWriter writer;
#endif
  writer.plotParticles(particles, out_name, iteration);
}
