#include "Log.hpp"
#include "Server.hpp"

#include <boost/asio/any_io_executor.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/cobalt/op.hpp>
#include <boost/cobalt/spawn.hpp>
#include <boost/cobalt/task.hpp>
#include <boost/system/detail/error_code.hpp>

#include <csignal>
#include <cstdint>
#include <exception>
#include <iostream>
#include <print>
#include <string_view>

namespace {

namespace asio = boost::asio;
namespace cobalt = boost::cobalt;
namespace system = boost::system;

using server::Server;

constexpr std::string_view kHost = "127.0.0.1";
constexpr uint16_t kPort = 8080;

using namespace std::string_literals;

[[nodiscard]]
cobalt::task<void> AwaitSignalAndStop(asio::io_context* ctx,
                                      asio::signal_set* signals, Server* srv,
                                      logger::Logger log) {
    system::error_code ec;
    const int sig =
        co_await signals->async_wait(asio::redirect_error(cobalt::use_op, ec));

    if (ec) {
        log.Warning("Signal wait failed: {}", ec.message());
        ctx->stop();
        co_return;
    }

    log.Info("Signal {} received, stopping server", sig);
    srv->Stop();
}

}  // namespace

// NOLINTNEXTLINE (bugprone-exception-escape)
int main() {
    try {
        // NOLINTNEXTLINE (misc-include-cleaner)
        logger::Logger log("server"s);
        asio::io_context ctx;

        Server srv{ctx.get_executor(), kHost, kPort, log};

        cobalt::spawn(ctx, srv.Run(), asio::detached);

        asio::signal_set signals{ctx, SIGINT, SIGTERM};
        cobalt::spawn(ctx, AwaitSignalAndStop(&ctx, &signals, &srv, log),
                      asio::detached);

        ctx.run();
    } catch (const std::exception& e) {
        std::println(std::cerr, "Fatal: {}", e.what());
    }

    return 0;
}
