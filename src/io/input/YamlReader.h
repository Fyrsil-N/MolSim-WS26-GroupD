//
// Created by fmathis on 13/11/2026.
//

#pragma once

#include "InputReader.h"
#include "ParticleContainer.h"
#include "ShapeParameters.h"
#include "SimulationParameters.h"
#include "yaml-cpp/yaml.h"

namespace io {

/**
 * This class implements functionality to fill a particle container with particles arranged in specific forms,
 * defined by a YAML file
 */
class YamlReader : public InputReader {
    /**
     * Name of the configuration file that stores the input data.
     */
    std::string filename;

    /**
     * First node of the configuration file
     */
    YAML::Node file;

    /**
     * Generates a cuboid based on given parameters.
     * @param params the shape parameters
     * @param particles ParticleContainer to store all the newly generated particles
     * @param third_dim Whether the third spatial dimension is enabled
     */
    static void generateCuboid(ShapeParameters &params, ParticleContainer &particles, bool third_dim);

    /**
     * Generates a particle based on given parameters.
     * @param params the shape parameters
     * @param particles ParticleContainer to store all the newly generated particles
     * @param third_dim Whether the third spatial dimension is enabled
     */
    static void generateParticle(ShapeParameters &params, ParticleContainer &particles, bool third_dim);

    /**
     * Generates an ellipsoid based on given parameters.
     * @param params the shape parameters
     * @param particles ParticleContainer to store all the newly generated particles
     * @param third_dim Whether the third spatial dimension is enabled
     */
    static void generateEllipsoid(ShapeParameters &params, ParticleContainer &particles, bool third_dim);

    /**
     * Parses all necessary parameters to create a shape from a given YAML root node
     * @param root the root node of the data to parse
     * @param third_dim Whether the third spatial dimension is enabled
     * @return a ShapeParameters struct containing all necessary information for generating the desired shape
     */
    static ShapeParameters parseParameters(YAML::Node root, bool third_dim);

    /**
     * Parses which pairwise force to be used for the simulation
     * @param root the root node of the data to parse
     * @param params SimulationParameters to write the results into
     */
    static void parsePairwiseForce(YAML::Node root, SimulationParameters &params);

    /**
     * Iterate over every instance of a shape and generate it using its dedicated generator function
     * @param f Callable taking a node and the ParticleContainer and fills the container with the shape information from
     * the node
     * @param root the root node of the shapes to parse
     * @param particles ParticleContainer to store all the newly generated particles
     * @param third_dim Whether the third spatial dimension is enabled
     */
    template <class F>
    static void generateShapes(const F &f, YAML::Node root, ParticleContainer &particles, bool third_dim) {
        for (auto iter = root.begin(); iter != root.end(); ++iter) {
            if (!iter->as<YAML::Node>(YAML::Node(YAML::NodeType::Undefined)).IsMap()) {
                spdlog::error("YamlReader Error: Couldn't parse shape information.");
                exit(EXIT_FAILURE);
            }

            ShapeParameters params = parseParameters(iter->as<YAML::Node>(), third_dim);
            f(params, particles, third_dim);
        }
    }

   public:
    /**
     * Constructor for a YamlReader for the given config file.
     * @param filename Name of the config file that stores the particle configuration
     */
    explicit YamlReader(std::string filename);

    /**
     * Read in global simulation settings like domain size, output interval, start and end time
     * @param params SimulationParameters to fill out
     */
    void readInSimulation(SimulationParameters &params) override;

    /**
     * Read in initial particle data from the input file of the class and store the new particles in the container.
     * @param particles ParticleContainer to store all the newly generated particles
     * @param third_dim Whether the third spatial dimension is enabled
     */
    void readInParticles(ParticleContainer &particles, bool third_dim) override;
};
}  // namespace io