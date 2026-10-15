#include "Client.hpp"
#include "Log.hpp"

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

using client::Client;
using logger::Logger;

constexpr std::string_view kHost = "127.0.0.1";
constexpr uint16_t kPort = 8080;

using namespace std::string_literals;

[[nodiscard]]
cobalt::task<void> AwaitSignal(asio::signal_set* signals, Client* client,
                               Logger log) {
    system::error_code ec;
    const int signum =
        co_await signals->async_wait(asio::redirect_error(cobalt::use_op, ec));
    if (ec) {
        log.Error("Failed to wait for signal: {}", ec.message());
        co_return;
    }

    log.Info("Received signal {}, stopping client...", signum);
    client->Close();
}

}  // namespace

// NOLINTNEXTLINE (bugprone-exception-escape)
int main() {
    try {
        asio::io_context ctx;
        // NOLINTNEXTLINE (misc-include-cleaner)
        Logger log("client"s);

        Client cli{ctx.get_executor(), kHost, kPort, log};

        asio::signal_set signals{ctx, SIGINT, SIGTERM};

        cobalt::spawn(ctx, cli.Run(), asio::detached);
        cobalt::spawn(ctx, AwaitSignal(&signals, &cli, log), asio::detached);

        ctx.run();
    } catch (const std::exception& e) {
        std::println(std::cerr, "Fatal: {}", e.what());
    }

    return 0;
}
