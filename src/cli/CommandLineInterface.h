//
// Created by hutj on 22.10.26.
//

#pragma once

#include <optional>

#include "SimulationParameters.h"

namespace cli {
/**
 * parse and validate command line arguments
 * @return the parameters or std::nullopt if the arguments are invalid
 */
std::optional<SimulationParameters> parseArguments(int argc, char *argv[]);
}  // namespace cli
