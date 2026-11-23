#pragma once

#include <stdexcept>
#include <string>

namespace io {
/**
 * Custom exception that input readers throw when the input file does not match the required format (e.g. missing
 * parameters or invalid input).
 * This replaces the old std::exit for better testability and all error messages now also have a common format.
 */
class InputError : public std::runtime_error {
   public:
    /**
     * @param filename name of the input file the error was found in
     * @param line line number where the error was found or <= 0 if unknown
     * @param message error message describing what went wrong
     */
    InputError(const std::string &filename, const int line, const std::string &message)
        : std::runtime_error(filename + (line > 0 ? ":" + std::to_string(line) : "") + ": " + message) {}
};
}  // namespace io
