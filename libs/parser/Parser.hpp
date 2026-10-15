#pragma once

#include <string>
#include <string_view>

namespace parser {

/**
 * @brief Removes leading whitespace characters
 * @param str Input string view
 * @return String view without leading whitespace
 */
[[nodiscard]] std::string_view TrimSpacesLeft(std::string_view str) noexcept;
/**
 * @brief Removes trailing whitespace characters
 * @param str Input string view
 * @return String view without trailing whitespace
 */
[[nodiscard]] std::string_view TrimSpacesRight(std::string_view str) noexcept;
/**
 * @brief Removes leading and trailing whitespace characters
 * @param str Input string view
 * @return String view without leading and trailing whitespace
 */
[[nodiscard]] std::string_view TrimSpaces(std::string_view str) noexcept;
/**
 * @brief Converts all characters in the string view to lowercase
 * @param str Input string view
 * @return String view with all characters in
 * lowercase
 */
[[nodiscard]] std::string ToLower(std::string_view str);

}  // namespace parser
