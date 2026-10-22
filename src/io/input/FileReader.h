/*
 * FileReader.h
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */

#pragma once

#include "ParticleContainer.h"

namespace io {

class FileReader {
 public:
  FileReader();
  virtual ~FileReader();

  void readFile(ParticleContainer &particles, char *filename);
};

}  // namespace io
