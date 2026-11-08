//
// Created by hutj on 08.11.26.
//

#pragma once
#include <charconv>
#include <cmath>
#include <optional>
#include <string_view>
#include <system_error>

/**
 * parse string to a finite double
 * @param str string_view of the string to parse
 * @return the parsed double if possible or a std::nullopt
 */
inline std::optional<double> parseDouble(std::string_view str) {
    double value{};
    const auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
    if (ec != std::errc{} || ptr != str.data() + str.size() || !std::isfinite(value)) {
        return std::nullopt;
    }
    return value;
}

/**
 * parse string to an int
 * @param str string_view of the string to parse
 * @return the parsed int if possible or a std::nullopt
 */
inline std::optional<int> parseInt(std::string_view str) {
    int value{};
    const auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
    if (ec != std::errc{} || ptr != str.data() + str.size()) {
        return std::nullopt;
    }
    return value;
}

/**
 * Remove leading and trailing whitespace from a string
 * @param str string_view of the string to trim
 * @return view of the trimmed part (empty if there is only whitespace)
 */
inline std::string_view trim(std::string_view str) {
    const auto first = str.find_first_not_of(" \t\r");
    if (first == std::string_view::npos) {
        return {};
    }
    const auto last = str.find_last_not_of(" \t\r");
    return str.substr(first, last - first + 1);
}