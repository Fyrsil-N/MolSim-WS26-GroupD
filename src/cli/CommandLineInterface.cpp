//
// Created by hutj on 22.10.26.
//

#include "cli/CommandLineInterface.h"

#include <getopt.h>
#include <spdlog/spdlog.h>

#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

#include "utils/Parsers.h"

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
              << "  -l, --log-level <level>        trace|debug|info|warn|err|critical|off (default: info)\n"
              << "  -f, --file-format <format>     input file format: raw|generator (default: generator)\n"
#ifdef ENABLE_VTK_OUTPUT
              << "  -v, --vtk                      output simulation files using vtk (default: xyz)\n"
#else
              << "  -v, --vtk                      output simulation files using vtk (unavailable in this build)\n"
#endif
#ifdef ENABLE_OPENMP
              << "  -p, --parallel                 calculate the forces in parallel (default: serial)\n"
#else
              << "  -p, --parallel                 calculate the forces in parallel (unavailable in this build)\n"
#endif
              << "  -b, --benchmark                run a benchmark (disables all output and logs)\n"
              << "  -h, --help                     print this help and exit\n";
}

/**
 * Parse the given log level and verify it is a proper log level.
 * @param str log level string provided by the user
 * @return valid and verified log level enum or a std::nullopt
 */
std::optional<spdlog::level::level_enum> parseLogLevel(std::string_view str) {
    spdlog::level::level_enum level = spdlog::level::from_str(std::string(str));
    // a wrong string results in level = off so check if the string was actually off or if its an error.
    if (level == spdlog::level::off && str != "off") {
        return std::nullopt;
    }
    return level;
}

/**
 * Parse the given input file format and verify it is supported.
 * @param str file format string provided by the user
 * @return the matching FileFormat or a std::nullopt
 */
std::optional<FileFormat> parseFileFormat(std::string_view str) {
    if (str == "raw") {
        return FileFormat::raw;
    }
    if (str == "generator") {
        return FileFormat::generator;
    }
    return std::nullopt;
}
}  // namespace

namespace cli {
std::optional<SimulationParameters> parseArguments(int argc, char *argv[]) {
    SimulationParameters params;

    const auto fail = [&](const std::string &message) -> std::optional<SimulationParameters> {
        spdlog::error("{}: {}", argv[0], message);
        spdlog::info("Try '{} --help' for more information.", argv[0]);
        return std::nullopt;
    };

    const option long_options[] = {
        {"delta-t", required_argument, nullptr, 'd'},
        {"t-end", required_argument, nullptr, 'e'},
        {"output-interval", required_argument, nullptr, 'o'},
        {"log-level", required_argument, nullptr, 'l'},
        {"file-format", required_argument, nullptr, 'f'},
        {"vtk", no_argument, nullptr, 'v'},
        {"parallel", no_argument, nullptr, 'p'},
        {"benchmark", no_argument, nullptr, 'b'},
        {"help", no_argument, nullptr, 'h'},
        {nullptr, 0, nullptr, 0},
    };

    opterr = 0;

    int opt = 0;
    while ((opt = getopt_long(argc, argv, ":d:e:o:l:f:vpbh", std::data(long_options), nullptr)) != -1) {
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
            case 'l': {
                const auto value = parseLogLevel(optarg);
                if (!value) {
                    return fail("invalid log-level '" + std::string(optarg) +
                                "', expected one of [trace, debug, info, warn, err, critical, off]");
                }
                params.log_level = *value;
                break;
            }
            case 'v': {
#ifdef ENABLE_VTK_OUTPUT
                params.vtk_output = true;
#else
                return fail(
                    "unsupported flag -v set. VTK is not available to the program (reconfigure with "
                    "-DENABLE_VTK_OUTPUT=ON to enable)");
#endif
                break;
            }
            case 'f': {
                const auto value = parseFileFormat(optarg);
                if (!value) {
                    return fail("invalid file-format '" + std::string(optarg) + "', expected one of [raw, generator]");
                }
                params.file_format = *value;
                break;
            }
            case 'p': {
#ifdef ENABLE_OPENMP
                params.omp_enabled = true;
#else
                return fail(
                    "unsupported flag -p set. OpenMP is not available to the program (reconfigure with "
                    "-DENABLE_OPENMP=ON to enable)");
#endif
                break;
            }
            case 'b': {
                params.benchmark_run = true;
                break;
            }
            case 'h': {
                printHelp(argv[0]);
                std::exit(EXIT_SUCCESS);
            }
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
        spdlog::warn("delta_t ({}) is larger than t_end ({})", params.delta_t, params.t_end);
    }

    return params;
}
}  // namespace cli
