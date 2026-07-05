#include <boost/asio.hpp>
#include <iostream>
#include "Globals.h"

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

int main() {
    asio::io_context io;

    tcp::acceptor acceptor{io, tcp::endpoint{tcp::v4(), Globals::REDIS_PORT}};

    std::cout << "ToyRedis listening on " << Globals::REDIS_HOST << ":" << Globals::REDIS_PORT << "\n";

    for (;;) {
        tcp::socket socket{io};
        // Blocks until a new connection is accepted
        acceptor.accept(socket);
        std::cout << "New connection\n";
    }
}
