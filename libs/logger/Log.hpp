#pragma once

#include "spdlog/common.h"
#include "spdlog/logger.h"

#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

namespace logger {

enum class Level : uint8_t {
    Info,
    Warning,
    Error,
    Fatal,
};

class Logger final {
 public:
    explicit Logger(std::string service_name,
                    std::filesystem::path log_root = "out/logs");

    void Log(Level level, std::string_view message) const;
    void Flush() const;

    template <typename... Args>
    void Info(spdlog::format_string_t<Args...> format, Args&&... args) const {
        CurrentLogger()->info(format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    void Warning(spdlog::format_string_t<Args...> format,
                 Args&&... args) const {
        CurrentLogger()->warn(format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    void Error(spdlog::format_string_t<Args...> format, Args&&... args) const {
        CurrentLogger()->error(format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    void Fatal(spdlog::format_string_t<Args...> format, Args&&... args) const {
        CurrentLogger()->critical(format, std::forward<Args>(args)...);
    }

 private:
    class State;

    [[nodiscard]] std::shared_ptr<spdlog::logger> CurrentLogger() const;

    std::shared_ptr<State> state_;
};

}  // namespace logger
