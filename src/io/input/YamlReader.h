//
// Created by fmathis on 13/11/2026.
//

#pragma once

#include "InputError.h"
#include "InputReader.h"
#include "ParticleContainer/ParticleContainer.h"
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
    YAML::Node root_node;

    /**
     * Throw an InputError for the given node with a message containing file name and line number of the error.
     * @param node the node the error occurred in
     * @param message description of the problem
     * @throws io::InputError
     */
    [[noreturn]] void fail(const YAML::Node &node, const std::string &message) const;

    /**
     * Parses which pairwise force to be used for the simulation
     * @param root the root node of the data to parse
     * @param params SimulationParameters to write the results into
     * @throws io::InputError
     */
    void parsePairwiseForce(YAML::Node root, SimulationParameters &params) const;

    /**
     * Parses the behavior of the lower and upper boundary of one axis
     * @param node node holding two strings, each "reflecting" or "outflow"
     * @return an array containing the boundary behavior at the smaller coordinate and the bigger coordinate
     * @throws io::InputError
     */
    std::array<BoundaryCondition, 2> parseBoundaryCondition(const YAML::Node &node) const;

    /**
     * Parses all necessary parameters for the simulation bounds
     * @param root the root node of the data to parse
     * @param params SimulationParameters to write the results into
     * @throws io::InputError
     */
    void parseDomainParams(YAML::Node root, SimulationParameters &params);

    /**
     * Parses all necessary parameters to create a shape from a given YAML root node
     * @param root the root node of the data to parse
     * @param third_dim Whether the third spatial dimension is enabled
     * @return a ShapeParameters struct containing all necessary information for generating the desired shape
     */
    ShapeParameters parseShapeParams(YAML::Node root, bool third_dim);

    /**
     * Iterate over every instance of a shape and generate it using its dedicated generator function
     * @param f Callable taking a node and the ParticleContainer and fills the container with the shape information from
     * the node
     * @param root the root node of the shapes to parse
     * @param particles ParticleContainer to store all the newly generated particles
     * @param third_dim Whether the third spatial dimension is enabled
     * @throws io::InputError
     */
    template <class F>
    void generateShapes(const F &f, YAML::Node root, ParticleContainer &particles, bool third_dim) {
        for (auto iter = root.begin(); iter != root.end(); ++iter) {
            if (!iter->as<YAML::Node>(YAML::Node(YAML::NodeType::Undefined)).IsMap()) {
                fail(*iter, "shape entry should be a map");
            }

            ShapeParameters params = parseShapeParams(iter->as<YAML::Node>(), third_dim);
            f(params, particles, third_dim);
        }
    }

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
    static void generateParticle(const ShapeParameters &params, ParticleContainer &particles, bool third_dim);

    /**
     * Generates an ellipsoid based on given parameters.
     * @param params the shape parameters
     * @param particles ParticleContainer to store all the newly generated particles
     * @param third_dim Whether the third spatial dimension is enabled
     */
    static void generateEllipsoid(ShapeParameters &params, ParticleContainer &particles, bool third_dim);

   public:
    /**
     * Constructor for a YamlReader for the given config file.
     * @param filename Name of the config file that stores the particle configuration
     * @throws io::InputError
     */
    explicit YamlReader(std::string filename);

    /**
     * Read in global simulation settings like domain size, output interval, start and end time
     * @param params SimulationParameters to fill out
     * @throws io::InputError
     */
    void readInSimulation(SimulationParameters &params) override;

    /**
     * Read in initial particle data from the input file of the class and store the new particles in the container.
     * @param particles ParticleContainer to store all the newly generated particles
     * @param third_dim Whether the third spatial dimension is enabled
     * @throws io::InputError
     */
    void readInParticles(ParticleContainer &particles, bool third_dim) override;
};
}  // namespace io