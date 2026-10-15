#include "Parser.hpp"

#include <string>
#include <string_view>
#include <ranges>

namespace {

constexpr std::string_view kSpaces = " \t\n\r\v\f\u00A0";

}  // namespace

namespace args {

std::string_view TrimSpacesLeft(std::string_view str) noexcept {
    const auto pos = str.find_first_not_of(kSpaces);
    if (pos == std::string_view::npos) [[unlikely]] {
        return {};
    }
    return str.substr(pos);
}

std::string_view TrimSpacesRight(std::string_view str) noexcept {
    const auto pos = str.find_last_not_of(kSpaces);
    if (pos == std::string_view::npos) [[unlikely]] {
        return {};
    }
    return str.substr(0, pos + 1);
}

std::string_view TrimSpaces(std::string_view str) noexcept {
    return TrimSpacesLeft(TrimSpacesRight(str));
}

std::string ToLower(std::string_view str) {
    return str | std::views::transform([](char c) constexpr {
               return (c >= 'A' && c <= 'Z')
                          ? static_cast<char>(c + ('a' - 'A'))
                          : c;
           }) |
           std::ranges::to<std::string>();
}

}  // namespace args
