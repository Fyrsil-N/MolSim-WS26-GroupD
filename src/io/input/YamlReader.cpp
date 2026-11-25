//
// Created by fmathis on 13/11/2026.
//

#include "YamlReader.h"

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <algorithm>
#include <array>
#include <string>
#include <utility>
#include <vector>

#include "InputError.h"
#include "ParticleGenerator.h"
#include "ShapeParameters.h"
#include "SimulationParameters.h"

/**
 * private namespace to encapsulate all the yaml-cpp from the rest of the project
 */
namespace {
using io::InputError;
/**
 * Class wrapping a YAML node to make checking the format and required / allowed keys easier.
 * This class also encapsulates all the yaml-cpp functionality to hide it from the rest of the project.
 * The key idea is that once finished is called all keys have been accounted for to make sure there are no extra invalid
 * key, value pairs in the yaml.
 */
class Section {
   private:
    /** the wrapped yaml map */
    YAML::Node node;

    /** filename of the config file for error messages only */
    const std::string filename;

    /** All the keys allowed / asked for in this section. finish() rejects every other key */
    std::vector<std::string> known_keys;

    /**
     * Look up a key in the node
     * @param key the key of the Node to look up
     */
    [[nodiscard]] YAML::Node lookup(const std::string &key) const { return std::as_const(node)[key]; }

    /**
     * Convert the value of a key to T
     * @param value Node to convert
     * @param key name of the key for error messages only
     * @throws InputError if the value can't be converted
     */
    template <class T>
    T convert(const YAML::Node &value, const std::string &key) const {
        try {
            return value.as<T>();
        } catch (const YAML::Exception &) {
            fail(value, "failed converting " + key + " to its target type");
        }
    }

   public:
    /**
     * Constructor of a Section from a yaml node
     * @param node the map to wrap
     * @param filename input file name for error messages
     * @throws InputError if the node is not a map
     */
    Section(const YAML::Node &node, const std::string &filename) : node(node), filename(filename) {
        if (!node.IsMap()) {
            fail(node, "should be a map");
        }
    }

    /**
     * Constructor of a Section from the root node of a yaml file
     * @param filename name of the yaml input file
     * @throws InputError if the file cannot be opened or does not contain a map
     */
    Section(const std::string &filename) : filename(filename) {
        try {
            node = YAML::LoadFile(this->filename);
        } catch (const YAML::Exception &e) {
            throw InputError(this->filename, e.mark.line + 1, e.msg);
        }

        if (!node.IsMap()) {
            fail(node, "should be a map");
        }
    }

    /**
     * Throw an InputError pointing at the the specified node
     * @param node_arg Node the program failed at
     * @param message description of the problem
     * @throws InputError
     */
    [[noreturn]] void fail(const YAML::Node &node_arg, const std::string &message) const {
        throw InputError(filename, node_arg.Mark().line + 1, message);
    }

    /**
     * Throw an InputError for this section in general
     * @param message description of the problem
     * @throws InputError
     */
    [[noreturn]] void fail(const std::string &message) const {
        throw InputError(filename, node.Mark().line + 1, message);
    }

    /**
     * Record that the key is allowed in this map without reading or requiring it.
     * @param key key to add to the allowed list
     */
    void markKnown(const std::string &key) {
        if (std::ranges::find(known_keys, key) == known_keys.end()) {
            known_keys.push_back(key);
        }
    }

    /**
     * Read a required value
     * @param key key of the value to be retrieved
     * @throws InputError if the key is missing or the value can't be converted to T
     */
    template <class T>
    T required(const std::string &key) {
        markKnown(key);
        const YAML::Node value = lookup(key);
        if (!value) {
            fail(node, "missing required key " + key);
        }
        return convert<T>(value, key);
    }

    /**
     * Read an optional value or return the fallback if the key is not present.
     * @param key key of the value to be retrieved
     * @param fallback value to use if the key is not found
     * @throws InputError if the value can't be converted to T
     */
    template <class T>
    T optional(const std::string &key, T fallback) {
        markKnown(key);
        const YAML::Node value = lookup(key);
        if (!value) {
            return fallback;
        }
        return convert<T>(value, key);
    }

    /**
     * Check whether the key holds a single value (used for pair_force: Gravity)
     * @param key key to check
     * @return true if the key exists and holds a scalar
     */
    bool isScalar(const std::string &key) {
        markKnown(key);
        const YAML::Node value = lookup(key);
        return value && value.IsScalar();
    }

    /**
     * @param key key of the subsection
     * @return the section of a required key
     * @throws InputError if the key is missing or its value is not a map
     */
    Section section(const std::string &key) {
        markKnown(key);
        YAML::Node value = lookup(key);
        if (!value) {
            fail(node, "missing required key " + key);
        }
        return Section{value, filename};
    }

    /**
     * @param key of the section list
     * @return List of sections stored under a given key, empty if the key does not exist
     * @throws InputError if the value is not a list or an element is not a map
     */
    std::vector<Section> getSectionList(const std::string &key) {
        markKnown(key);
        const YAML::Node value = lookup(key);
        if (!value) {
            return {};
        }

        if (!value.IsSequence()) {
            fail(value, key + " should be a list");
        }
        std::vector<Section> result;
        result.reserve(value.size());
        for (const auto &element : value) {
            result.emplace_back(element, filename);
        }
        return result;
    }

    /**
     * Reject every key of the map that was never asked for. Call after all keys of the map have been read.
     * @throws InputError for the first unknown key
     */
    void finish() const {
        for (auto iter = node.begin(); iter != node.end(); ++iter) {
            const auto key = iter->first.as<std::string>();
            if (std::ranges::find(known_keys, key) != known_keys.end()) {
                continue;
            }

            std::string allowed;
            for (const auto &known : known_keys) {
                allowed += (allowed.empty() ? "" : ", ") + known;
            }
            fail(iter->first, "unknown key " + key + " (allowed: " + allowed + ")");
        }
    }
};

/**
 * Parses which pairwise force to be used for the simulation
 * @param simulation the simulation section holding the pair_force key
 * @param params SimulationParameters to write the results into
 * @throws io::InputError
 */
void parsePairwiseForce(Section &simulation, SimulationParameters &params) {
    // gravity
    if (simulation.isScalar("pair_force")) {
        const auto name = simulation.required<std::string>("pair_force");
        if (name != "Gravity") {
            simulation.fail("unknown pair_force '" + name + "' (expected Gravity or a Lennard-Jones map)");
        }
        params.pair_force = GRAVITY;
        return;
    }

    // lennard jones
    Section force_section = simulation.section("pair_force");
    Section lennard_jones_section = force_section.section("Lennard-Jones");

    params.pair_force = LENNARD_JONES;
    params.sigma = lennard_jones_section.required<double>("sigma");
    params.epsilon = lennard_jones_section.required<double>("epsilon");
    params.r_cutoff = lennard_jones_section.optional<double>("cutoff", -1);

    lennard_jones_section.finish();
    force_section.finish();

    // value validation
    if (params.sigma <= 0) {
        lennard_jones_section.fail("Sigma is expected to be a strictly positive number");
    }
    if (params.epsilon <= 0) {
        lennard_jones_section.fail("Epsilon is expected to be a strictly positive number");
    }
    if (params.r_cutoff <= 0) {
        spdlog::info("The cutoff radius is disabled.");
    }
    if (params.r_cutoff <= 2 * params.sigma) {
        spdlog::warn(
            "The cutoff radius is set to a very small value so Lennard Jones Forces will probably not act as intended");
    }
}

/**
 * Parses the behavior of the lower and upper boundary of one axis
 * @param boundaries the boundaries section
 * @param axis key of the axis ("x", "y" or "z")
 * @return an array containing the boundary behavior at the smaller coordinate and the bigger coordinate
 * @throws io::InputError
 */
std::array<BoundaryCondition, 2> parseBoundaryCondition(Section &boundaries, const std::string &axis) {
    const auto names = boundaries.required<std::array<std::string, 2>>(axis);
    std::array<BoundaryCondition, 2> result{};

    for (std::size_t i = 0; i < names.size(); i++) {
        if (names.at(i) == "reflecting") {
            result.at(i) = REFLECTING;
        } else if (names.at(i) == "outflow") {
            result.at(i) = OUTFLOW;
        } else {
            boundaries.fail("unknown boundary condition '" + names.at(i) + "' (expected reflecting or outflow)");
        }
    }

    return result;
}

/**
 * Parses all necessary parameters for the simulation bounds
 * @param domain the domain section
 * @param params SimulationParameters to write the results into
 * @throws io::InputError
 */
void parseDomainParams(Section &domain, SimulationParameters &params) {
    params.domain_anchor = domain.required<std::array<double, 3>>("anchor_point");
    params.domain_dimensions = domain.required<std::array<double, 3>>("dimensions");

    Section boundaries = domain.section("boundaries");

    // third dimension is already parsed at this point so can be used it without danger
    if (params.third_dim) {
        params.boundary_conditions[0] = parseBoundaryCondition(boundaries, "x");
        params.boundary_conditions[1] = parseBoundaryCondition(boundaries, "y");
        params.boundary_conditions[2] = parseBoundaryCondition(boundaries, "z");
    } else {
        params.boundary_conditions[0] = parseBoundaryCondition(boundaries, "x");
        params.boundary_conditions[1] = parseBoundaryCondition(boundaries, "y");
    }
    boundaries.finish();

    domain.finish();

    if (!params.third_dim) {
        params.domain_anchor.at(2) = 0;
        params.domain_dimensions.at(2) = 1;
    }
}

/**
 * Parses all necessary parameters to create a shape (some are not allowed for single particles)
 * @param shape the section of one shape
 * @param multiple_particles flag to tell if particle_dims and spacing are needed parameters
 * @param third_dim Whether the third spatial dimension is enabled
 * @return the shape parameters, dimensions and spacing are left at their defaults
 */
ShapeParameters parseShapeParams(Section &shape, const bool multiple_particles, const bool third_dim) {
    ShapeParameters params;

    params.anchor_point = shape.required<std::array<double, 3>>("anchor_point");
    params.velocity = shape.required<std::array<double, 3>>("velocity");
    params.particle_mass = shape.required<double>("mass");
    params.type = shape.optional<int>("type", 0);

    // particle dims and spacing only make sense for cuboids or ellipsoids etc.
    if (multiple_particles) {
        params.particle_dims = shape.required<std::array<std::size_t, 3>>("dimensions");
        params.spacing = shape.required<double>("spacing");
    }

    if (!third_dim) {
        params.anchor_point.at(2) = 0;
        params.velocity.at(2) = 0;
        params.particle_dims.at(2) = 1;
    }

    shape.finish();

    // value validation
    if (params.particle_mass <= 0) {
        shape.fail("mass is expected to be a strictly positive number");
    }
    if (params.spacing <= 0) {
        shape.fail("spacing is expected to be a strictly positive number");
    }

    return params;
}

}  // namespace

namespace io {
YamlReader::YamlReader(std::string filename_arg) : filename(std::move(filename_arg)) {
    // only check the top level keys here, each readIn* function requires its own section
    Section root = Section(filename);
    root.markKnown("simulation");
    root.markKnown("scene");
    root.finish();
}

void YamlReader::readInSimulation(SimulationParameters &params) {
    Section root = Section(filename);
    Section simulation = root.section("simulation");

    params.output_file_basename = simulation.optional<std::string>("basename", "MD_output");
    params.delta_t = simulation.required<double>("delta_t");
    params.t_end = simulation.required<double>("end_t");
    params.output_interval = simulation.optional<int>("out_interval", 10);
    // has to be read before the domain
    params.third_dim = simulation.optional<bool>("3d", false);

    Section domain = simulation.section("domain");
    parseDomainParams(domain, params);
    parsePairwiseForce(simulation, params);

    simulation.finish();

    // value validation
    if (params.delta_t <= 0) {
        simulation.fail("delta_t is expected to be a strictly positive number");
    }
    if (params.t_end <= 0) {
        simulation.fail("end_t is expected to be a strictly positive number");
    }
    if (params.output_interval <= 0) {
        simulation.fail("out_interval is expected to be a strictly positive number");
    }
    if (params.delta_t > params.t_end) {
        spdlog::warn("delta_t ({}) is larger than t_end ({})", params.delta_t, params.t_end);
    }
}

void YamlReader::readInParticles(ParticleContainer &particles, const bool third_dim) {
    Section root = Section(filename);
    Section scene = root.section("scene");

    // collect all the lists of allowed shapes / particle objects
    std::vector<Section> cuboids = scene.getSectionList("cuboids");
    std::vector<Section> single_particles = scene.getSectionList("particles");
    std::vector<Section> ellipsoids = scene.getSectionList("ellipsoids");
    scene.finish();

    for (Section &shape : single_particles) {
        ParticleGenerator::generateParticle(parseShapeParams(shape, false, third_dim), particles, third_dim);
    }
    for (Section &shape : cuboids) {
        ParticleGenerator::generateCuboid(parseShapeParams(shape, true, third_dim), particles, third_dim);
    }
    for (Section &shape : ellipsoids) {
        ParticleGenerator::generateEllipsoid(parseShapeParams(shape, true, third_dim), particles, third_dim);
    }

    particles.shrinkToFit();
}
}  // namespace io
