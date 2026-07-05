#include "Globals.h"
#include <boost/asio.hpp>
#include <iostream>
#include <string>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

int main() {
    asio::io_context io;

    const std::string host { Globals::REDIS_HOST };
    const std::string port { std::to_string(Globals::REDIS_PORT) };

    tcp::resolver resolver {io};
    const auto endpoints = resolver.resolve(host, port);

    tcp::socket socket{io};
    asio::connect(socket, endpoints);

    std::cout << "Connected to " << host << ":" << port << "\n";
    return 0;
}
