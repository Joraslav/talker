#include "Log.hpp"

#include "spdlog/common.h"
#include "spdlog/logger.h"
#include "spdlog/sinks/basic_file_sink.h"

#include <chrono>
#include <filesystem>
#include <format>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>

namespace logger {

namespace fs = std::filesystem;
using namespace std::string_literals;

namespace {

[[nodiscard]] std::string CurrentData() {
    const auto now =
        // NOLINTNEXTLINE (misc-include-cleaner)
        std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now());
    return std::format("{:%Y-%m-%d}", now);
}
}  // namespace

class Logger::State final {
 public:
    explicit State(std::string service_name, fs::path log_root)
        : service_name_(std::move(service_name)),
          log_root_(std::move(log_root)) {}

 private:
    friend class Logger;
    std::string service_name_;
    fs::path log_root_;
    std::mutex mutex_;
    std::shared_ptr<spdlog::logger> logger_;
    std::string active_date_;
};

Logger::Logger(std::string service_name, fs::path log_root)
    : state_(std::make_shared<State>(std::move(service_name),
                                     std::move(log_root))) {
    const fs::path service_path{state_->service_name_};
    if (state_->service_name_.empty() || service_path.is_absolute() ||
        service_path.has_parent_path() || state_->service_name_ == "." ||
        state_->service_name_ == "..") {
        throw std::invalid_argument(
            "Logger service name must be single path component");
    }
    std::ignore = CurrentLogger();
}

std::shared_ptr<spdlog::logger> Logger::CurrentLogger() const {
    const std::string date = CurrentData();
    const std::scoped_lock<std::mutex> lock{state_->mutex_};
    if (state_->logger_ && state_->active_date_ == date) {
        return state_->logger_;
    }
    if (!fs::exists(state_->log_root_)) {
        fs::create_directories(state_->log_root_);
    }
    const fs::path file_path{state_->log_root_ / (date + ".log")};
    auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
        file_path.string(), false);
    auto logger = std::make_shared<spdlog::logger>(state_->service_name_, sink);
    // NOLINTNEXTLINE (misc_include)
    logger->set_pattern("%Y-%m-%d %H:%M:%S.%e [%^%l%$] [%n] %v"s);
    logger->set_level(spdlog::level::trace);
    logger->flush_on(spdlog::level::trace);
    state_->active_date_ = date;
    state_->logger_ = std::move(logger);
    return state_->logger_;
}

void Logger::Log(Level level, std::string_view message) const {
    const auto logger = CurrentLogger();
    switch (level) {
        case Level::Info:
            logger->info("{}", message);
            break;
        case Level::Warning:
            logger->warn("{}", message);
            break;
        case Level::Error:
            logger->error("{}", message);
            break;
        case Level::Fatal:
            logger->critical("{}", message);
            break;
    }
}

void Logger::Flush() const { CurrentLogger()->flush(); }

}  // namespace logger
