#include "Client.hpp"

#include "Error.hpp"
#include "Result.hpp"

#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <boost/cobalt/op.hpp>
#include <boost/cobalt/task.hpp>
#include <boost/system/detail/error_code.hpp>

#include <expected>
#include <iostream>
#include <istream>
#include <print>
#include <string>
#include <string_view>

namespace client {

namespace system = boost::system;

using common::Error;
using common::ErrorCode;
using common::Result;

cobalt::task<Result<void>> Client::Connect() {
    system::error_code ec;
    co_await socket_.async_connect(endpoint_,
                                   asio::redirect_error(cobalt::use_op, ec));
    if (ec) [[unlikely]] {
        co_return std::unexpected(Error{
            .code = ErrorCode::Network,
            .message = ec.message(),
        });
    }
    log_.Info("Connected to {}:{}", endpoint_.address().to_string(),
              endpoint_.port());
    co_return {};
}

cobalt::task<Result<std::string>> Client::Request(std::string_view message) {
    system::error_code ec;
    std::string payload{message};
    payload.push_back('\n');
    co_await asio::async_write(socket_, asio::buffer(payload),
                               asio::redirect_error(cobalt::use_op, ec));
    if (ec) [[unlikely]] {
        co_return std::unexpected(Error{
            .code = ErrorCode::Network,
            .message = ec.message(),
        });
    }

    asio::streambuf buffer;
    co_await asio::async_read_until(socket_, buffer, '\n',
                                    asio::redirect_error(cobalt::use_op, ec));
    if (ec) [[unlikely]] {
        co_return std::unexpected(Error{
            .code = ErrorCode::Network,
            .message = ec.message(),
        });
    }

    std::istream is{&buffer};
    std::string line;
    std::getline(is, line);
    co_return line;
}

cobalt::task<Result<void>> Client::Run() {
    const auto connectiont_result = co_await Connect();
    if (!connectiont_result) [[unlikely]] {
        co_return std::unexpected(connectiont_result.error());
    }

    asio::streambuf buffer;
    system::error_code ec;
    while (socket_.is_open()) {
        co_await asio::async_read_until(
            stdin_, buffer, '\n', asio::redirect_error(cobalt::use_op, ec));
        if (ec) {
            if (ec != asio::error::operation_aborted) {
                log_.Error("Read error: {}", ec.message());
            }
            break;
        }

        std::istream is{&buffer};
        std::string message;
        std::getline(is, message);
        if (message.empty()) {
            continue;
        }
        const auto request_result = co_await Request(message);
        if (!request_result) [[unlikely]] {
            log_.Error("Request error: {}", request_result.error().message);
            continue;
        }
        log_.Info("Response: {}", request_result.value());
        std::println("Server: {}", *request_result);
    }

    co_return {};
}

void Client::Close() noexcept {
    system::error_code ec;
    ec = stdin_.cancel(ec);
    if (ec) {
        log_.Warning("Cancel error: {}", ec.message());
    }
    if (socket_.is_open()) {
        ec = socket_.shutdown(tcp::socket::shutdown_both, ec);
        if (ec && ec != asio::error::not_connected) {
            log_.Warning("Shutdown error: {}", ec.message());
        }
        ec = {};
        ec = socket_.close(ec);
        if (ec) {
            log_.Warning("Close error: {}", ec.message());
        }
    }
}

}  // namespace client
