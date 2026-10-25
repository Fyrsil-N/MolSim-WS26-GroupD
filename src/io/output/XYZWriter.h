/*
 * XYZWriter.h
 *
 *  Created on: 01.03.2010
 *      Author: eckhardw
 */

#pragma once

#include "OutputWriter.h"
#include "ParticleContainer.h"

namespace io {

/**
 * This class implements the functionality to generate xyz output from particles.
 */
class XYZWriter : public OutputWriter {
 private:
  /**
   * Output filenames are of the form output_filename_<current_iteration>.xyz
   */
  std::string output_filename;

 public:
  /**
   * Constructor for the XYZ Writer
   * @param output_interval Output is written every output_interval iterations
   * @param filename Prefix of all output files
   */
  XYZWriter(int output_interval, std::string filename);

  /**
   * Write XYZ output of particles.
   * @param particles Particles to add to the output
   * @param iteration Current iteration number
   * @param time Current in-simulation time
   */
  void onStep(const ParticleContainer &particles, int iteration, double time) override;
};

}  // namespace io
