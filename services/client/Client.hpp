#pragma once

#include "Log.hpp"
#include "Result.hpp"

#include <unistd.h>

#include <boost/asio/any_io_executor.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/cobalt/task.hpp>

#include <cstdint>
#include <string>
#include <string_view>
#include <utility>

namespace client {

namespace asio = boost::asio;
namespace cobalt = boost::cobalt;
using asio::ip::tcp;

class Client final {
 public:
    Client(const asio::any_io_executor& ex, std::string_view host,
           uint16_t port, logger::Logger log)
        : socket_(ex),
          stdin_(socket_.get_executor(), ::dup(STDIN_FILENO)),
          endpoint_(asio::ip::make_address(host), port),
          log_(std::move(log)) {}

    [[nodiscard]]
    cobalt::task<common::Result<void>> Run();

    void Close() noexcept;

 private:
    [[nodiscard]]
    cobalt::task<common::Result<void>> Connect();

    [[nodiscard]]
    cobalt::task<common::Result<std::string>> Request(std::string_view message);

    tcp::socket socket_;
    asio::posix::stream_descriptor stdin_;
    tcp::endpoint endpoint_;
    logger::Logger log_;
};

}  // namespace client
