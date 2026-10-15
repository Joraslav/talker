#pragma once

#include "Log.hpp"
#include "Result.hpp"

#include <boost/asio/any_io_executor.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/cobalt/op.hpp>
#include <boost/cobalt/spawn.hpp>
#include <boost/cobalt/task.hpp>
#include <boost/system/detail/error_code.hpp>
#include <boost/system/system_error.hpp>

#include <cstdint>
#include <memory>
#include <string_view>
#include <utility>

namespace server {

namespace asio = boost::asio;
namespace cobalt = boost::cobalt;
using asio::ip::tcp;

using common::Result;

class Session final : public std::enable_shared_from_this<Session> {
 public:
    explicit Session(tcp::socket socket) : socket_(std::move(socket)) {}

    [[nodiscard]]
    cobalt::task<Result<void>> Run();
    [[nodiscard]]
    tcp::socket& GetSocket() noexcept;

 private:
    [[nodiscard]] cobalt::task<Result<void>> Handle(std::string_view request);

    tcp::socket socket_;
};

class Server final {
 public:
    Server(const asio::any_io_executor& ex, std::string_view host,
           uint16_t port, logger::Logger log)
        : acceptor_(ex, tcp::endpoint(asio::ip::make_address(host), port)),
          log_(std::move(log)) {}

    cobalt::task<void> Run();
    void Stop();

 private:
    tcp::acceptor acceptor_;
    logger::Logger log_;
};

}  // namespace server
