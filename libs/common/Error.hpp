#pragma once

#include "fmt/format.h"

#include <fmt/base.h>

#include <array>
#include <cstdint>
#include <format>
#include <string>
#include <string_view>
#include <utility>

namespace common {

enum class ErrorCode : uint8_t {
    Network,
    Protocol,
    Configuration,
    Resource,
    Environment,
    Internal,
};

struct Error final {
    ErrorCode code;
    std::string message;
};

}  // namespace common

template <>
struct std::formatter<common::ErrorCode> : std::formatter<std::string_view> {
    auto format(common::ErrorCode ec, std::format_context& ctx) const {
        constexpr std::array names{
            "Network",  "Protocol",    "Configuration",
            "Resource", "Environment", "Internal",
        };
        auto idx = std::to_underlying(ec);
        std::string_view name =
            (idx < names.size()) ? names.at(idx) : "Unknown";
        return std::formatter<std::string_view>::format(name, ctx);
    }
};

template <>
struct fmt::formatter<common::ErrorCode> : fmt::formatter<std::string_view> {
    auto format(common::ErrorCode ec, fmt::format_context& ctx) const {
        constexpr std::array names{
            "Network",  "Protocol",    "Configuration",
            "Resource", "Environment", "Internal",
        };
        auto idx = std::to_underlying(ec);
        std::string_view name =
            (idx < names.size()) ? names.at(idx) : "Unknown";
        return fmt::formatter<std::string_view>::format(name, ctx);
    }
};
