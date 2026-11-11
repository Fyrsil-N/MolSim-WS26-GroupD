//
// Created by luka-maric on 31.10.26.
//

#ifndef MOLSIM_GROUPD_PARTICLECREATOR_H
#define MOLSIM_GROUPD_PARTICLECREATOR_H

#include "InputReader.h"
#include "ParticleContainer.h"
#include "ShapeParameters.h"

namespace io {
/**
 * This class implements functionality to fill a particle container with particles arranged in specific forms,
 * defined by a configuration file
 */
class ParticleCreator : public InputReader {
   private:
    /**
     * Name of the configuration file that stores the initial particle data.
     */
    std::string config_filename;

   public:
    /**
     * Constructor for a ParticleCreator for the given config file.
     * @param filename Name of the config file that stores the particle configuration
     */
    explicit ParticleCreator(std::string filename);

    /**
     * Read in initial particle data from the input file of the class and store the new particles in the container.
     * @param particles ParticleContainer to store all the newly generated particles
     */
    void readInParticles(ParticleContainer &particles) override;

    /**
     * Generates a cuboid based on given parameters.
     * @param input_file the file input stream to read the parameters from
     * @param particles ParticleContainer to store all the newly generated particles
     */
    static void parseCuboid(std::ifstream &input_file, ParticleContainer &particles);

    /**
     * Generates an ellipsoid based on given parameters.
     * @param input_file the file input stream to read the parameters from
     * @param particles ParticleContainer to store all the newly generated particles
     */
    static void parseEllipsoid(std::ifstream &input_file, ParticleContainer &particles);

    /**
     * Generates a particle based on given parameters.
     * @param input_file the file input stream to read the parameters from
     * @param particles ParticleContainer to store the newly generated particle
     */
    static void parseParticle(std::ifstream &input_file, ParticleContainer &particles);

    /**
     * Parses all necessary parameters to create a shape from a given file input stream
     * @param input_file the file input stream to read the parameters from
     * @return a ShapeParameters struct containing all necessary information for generating the desired shape
     */
    static ShapeParameters parseParameters(std::ifstream &input_file);

    /**
     * Takes a line of coordinates in the form of 'x y z' and puts them into the given array.
     * @param array_to_fill Array to put the given coordinates into
     * @param line string line containing the needed coordinates in a string format
     */
    static void convertStringToArray(std::array<double, 3> &array_to_fill, const std::string &line);
};
}  // namespace io

#endif  // MOLSIM_GROUPD_PARTICLECREATOR_H
