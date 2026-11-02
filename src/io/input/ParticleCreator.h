//
// Created by luka-maric on 31.10.26.
//

#ifndef MOLSIM_GROUPD_PARTICLECREATOR_H
#define MOLSIM_GROUPD_PARTICLECREATOR_H

#include "InputReader.h"
#include "ParticleContainer.h"

namespace io
{
    /**
     * This class implements functionality to fill a particle container with particles arranged in specific forms,
     * defined by a configuration file
     */
    class ParticleCreator : InputReader {
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
         * Takes a line of coordinates in the form of 'x y z' and puts them into the given array.
         * @param array_to_fill Array to put the given coordinates into
         * @param line string line containing the needed coordinates in a string format
         */
        static void convertStringToArray(std::array<double, 3> &array_to_fill, const std::string &line);

        /**
         * Takes two double arrays and normalizes them by putting the smallest values of the same index into a and the biggest into b
         * @param a array that should contain the smaller values
         * @param b array that should contain the bigger values
         */
        static void normalizeCoordinateArrays(std::array<double, 3> &a, std::array<double, 3> &b);
    };
}

#endif //MOLSIM_GROUPD_PARTICLECREATOR_H
