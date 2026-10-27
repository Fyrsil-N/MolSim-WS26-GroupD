//
// Created by hutj on 22.10.26.
//

#pragma once

#include <optional>

#include "SimulationParameters.h"

/**
 * Namespace cli groups together all functions and helper needed to parse and process command line arguments and
 * options.
 */
namespace cli {
/**
 * Parse and validate command line arguments.
 * @param argc argc from main
 * @param argv argv from main
 * @return the parameters or std::nullopt if the arguments are invalid
 */
std::optional<SimulationParameters> parseArguments(int argc, char *argv[]);
}  // namespace cli
