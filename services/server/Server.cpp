#include "Server.hpp"

#include "Error.hpp"

#include <boost/asio/buffer.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <boost/cobalt/op.hpp>
#include <boost/cobalt/spawn.hpp>
#include <boost/cobalt/task.hpp>
#include <boost/system/detail/error_code.hpp>
#include <boost/system/system_error.hpp>

#include <expected>
#include <istream>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

namespace server {

namespace system = boost::system;

using common::Error;
using common::ErrorCode;

constexpr std::string_view kDefaultResponse = "We worked on this\n";
constexpr std::string_view kResponse = "Hi, I`m Server!\n";

cobalt::task<Result<void>> Session::Handle(std::string_view request) {
    using namespace std::string_view_literals;

    system::error_code ec;
    if (request == "hi"sv || request == "Hi"sv) {
        co_await asio::async_write(socket_, asio::buffer(kResponse),
                                   asio::redirect_error(cobalt::use_op, ec));
    } else {
        co_await asio::async_write(socket_, asio::buffer(kDefaultResponse),
                                   asio::redirect_error(cobalt::use_op, ec));
    }

    if (ec) [[unlikely]] {
        co_return std::unexpected(Error{
            .code = ErrorCode::Network,
            .message = ec.message(),
        });
    }
    co_return {};
}

cobalt::task<Result<void>> Session::Run() {
    auto self = shared_from_this();

    system::error_code ec;
    asio::streambuf buffer;
    while (socket_.is_open()) {
        co_await asio::async_read_until(
            socket_, buffer, '\n', asio::redirect_error(cobalt::use_op, ec));
        if (ec) {
            if (ec == asio::error::operation_aborted || ec == asio::error::eof)
                [[likely]] {
                co_return {};
            }
            co_return std::unexpected(Error{
                .code = ErrorCode::Network,
                .message = ec.message(),
            });
        }

        std::istream is{&buffer};
        std::string line;
        std::getline(is, line);

        const auto result = co_await Handle(line);
        if (!result) {
            co_return result;
        }
    }
    co_return {};
}

tcp::socket& Session::GetSocket() noexcept { return socket_; }

cobalt::task<void> Server::Run() {
    const auto& endpoint = acceptor_.local_endpoint();
    log_.Info("Server listening on {}:{}", endpoint.address().to_string(),
              endpoint.port());

    while (acceptor_.is_open()) {
        try {
            auto socket = co_await acceptor_.async_accept(cobalt::use_op);
            auto session = std::make_shared<Session>(std::move(socket));
            cobalt::spawn(acceptor_.get_executor(), session->Run(),
                          asio::detached);
        } catch (const system::system_error& e) {
            if (e.code() == asio::error::operation_aborted) {
                log_.Info("Acceprot closed");
                break;
            }
            log_.Error("Accept failed: {}", e.what());
        }
    }
}

void Server::Stop() {
    system::error_code ec;
    ec = acceptor_.close(ec);
    if (ec) {
        log_.Warning("Acceptor close error: {}", ec.message());
    }
}

}  // namespace server
