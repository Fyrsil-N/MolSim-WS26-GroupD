/*
 * VTKWriter.h
 *
 *  Created on: 01.03.2010
 *      Author: eckhardw
 */

#pragma once
#ifdef ENABLE_VTK_OUTPUT

#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>

#include <string>

#include "OutputWriter.h"
#include "ParticleContainer.h"

namespace io {

/**
 * This class implements the functionality to generate vtk output from
 * particles using the official VTK library.
 */
class VTKWriter : public OutputWriter {
   private:
    /**
     * Output filenames are of the form output_filename_<current_iteration>.vtu
     */
    std::string output_filename;

   public:
    /**
     * Constructor for the VTK Writer
     * @param output_interval Output is written every output_interval iterations
     * @param filename Prefix of all output files
     */
    VTKWriter(int output_interval, std::string filename);

    // Delete copy constructor and assignment operator
    VTKWriter(const VTKWriter &) = delete;
    VTKWriter &operator=(const VTKWriter &) = delete;

    /**
     * Write VTK output of particles.
     * @param particles Particles to add to the output
     * @param iteration Current iteration number
     * @param time Current in-simulation time
     */
    void onStep(const ParticleContainer &particles, int iteration, double time) override;
};

}  // namespace io
#endif