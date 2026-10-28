//
// Created by hutj on 22.10.26.
//

#include "cli/CommandLineInterface.h"

#include <getopt.h>

#include <charconv>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

namespace {
/**
 * print the help message for the program
 */
void printHelp(const char *program_name) {
    const SimulationParameters defaults;
    std::cout << "Usage: " << program_name << " [options] <input_file>\n"
              << "\n"
              << "Options:\n"
              << "  -d, --delta-t <double>         time step size (default: " << defaults.delta_t << ")\n"
              << "  -e, --t-end <double>           end time (default: " << defaults.t_end << ")\n"
              << "  -o, --output-interval <int>    output is written every N iterations (default: "
              << defaults.output_interval << ")\n"
#ifdef ENABLE_VTK_OUTPUT
              << "  -v, --vtk                      set vtk output flag (default: xyz)\n"
#endif
              << "  -h, --help                     print this help and exit\n";
}

/**
 * parse string (command line argument) to a finite double
 * @param str string_view of the string to parse
 * @return the parsed double if possible or a std::nullopt
 */
std::optional<double> parseDouble(std::string_view str) {
    double value{};
    const auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
    if (ec != std::errc{} || ptr != str.data() + str.size() || !std::isfinite(value)) {
        return std::nullopt;
    }
    return value;
}

/**
 * parse string (command line argument) to a finite int
 * @param str string_view of the string to parse
 * @return the parsed int if possible or a std::nullopt
 */
std::optional<int> parseInt(std::string_view str) {
    int value{};
    const auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
    if (ec != std::errc{} || ptr != str.data() + str.size()) {
        return std::nullopt;
    }
    return value;
}
}  // namespace

namespace cli {
std::optional<SimulationParameters> parseArguments(int argc, char *argv[]) {
    SimulationParameters params;

    const auto fail = [&](const std::string &message) -> std::optional<SimulationParameters> {
        std::cerr << argv[0] << ": " << message << '\n' << "Try '" << argv[0] << " --help' for more information.\n";
        return std::nullopt;
    };

    const option long_options[] = {
        {"delta-t", required_argument, nullptr, 'd'},
        {"t-end", required_argument, nullptr, 'e'},
        {"output-interval", required_argument, nullptr, 'o'},
#ifdef ENABLE_VTK_OUTPUT
        {"vtk", no_argument, nullptr, 'v'},
#endif
        {"help", no_argument, nullptr, 'h'},
        {nullptr, 0, nullptr, 0},
    };

    opterr = 0;

    int opt = 0;
    while ((opt = getopt_long(argc, argv, ":d:e:o:vh", std::data(long_options), nullptr)) != -1) {
        switch (opt) {
            case 'd': {
                const auto value = parseDouble(optarg);
                if (!value || *value <= 0) {
                    return fail("invalid delta_t '" + std::string(optarg) + "', expected a number > 0");
                }
                params.delta_t = *value;
                break;
            }
            case 'e': {
                const auto value = parseDouble(optarg);
                if (!value || *value <= 0) {
                    return fail("invalid t_end '" + std::string(optarg) + "', expected a number > 0");
                }
                params.t_end = *value;
                break;
            }
            case 'o': {
                const auto value = parseInt(optarg);
                if (!value || *value < 1) {
                    return fail("invalid output interval '" + std::string(optarg) + "', expected an integer >= 1");
                }
                params.output_interval = *value;
                break;
            }
#ifdef ENABLE_VTK_OUTPUT
            case 'v':
                params.vtk_output = true;
                break;
#endif
            case 'h':
                printHelp(argv[0]);
                std::exit(EXIT_SUCCESS);
            case ':':
                return fail("option '" + std::string(argv[optind - 1]) + "' requires an argument");
            default:
                return fail(
                    "unknown option '" +
                    (optopt != 0 ? std::string("-") + static_cast<char>(optopt) : std::string(argv[optind - 1])) + "'");
        }
    }

    if (argc - optind != 1) {
        return fail("expected exactly one input file, got " + std::to_string(argc - optind));
    }
    params.input_file = argv[optind];

    if (!std::filesystem::is_regular_file(params.input_file)) {
        return fail("input file '" + params.input_file + "' not found");
    }

    if (params.delta_t > params.t_end) {
        std::cerr << "Warning: delta_t (" << params.delta_t << ") is larger than t_end (" << params.t_end << ").\n";
    }

    return params;
}
}  // namespace cli
