/*
 * XYZWriter.h
 *
 *  Created on: 01.03.2010
 *      Author: eckhardw
 */

#pragma once

#include <fstream>

#include "ParticleContainer.h"

namespace io {

class XYZWriter {
 public:
  XYZWriter();

  virtual ~XYZWriter();

  void plotParticles(ParticleContainer &particles, const std::string &filename, int iteration);
};

}  // namespace io
