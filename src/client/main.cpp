#include "Global.h"
#include "RESP.h"

#include <boost/asio.hpp>
#include <iostream>
#include <string>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

std::string readResponse(tcp::socket& socket) {
  asio::streambuf buffer;
  asio::read_until(socket, buffer, "\r\n");
  return {asio::buffers_begin(buffer.data()), asio::buffers_end(buffer.data())};
}

int main() {
  asio::io_context io;

  const std::string host{Global::REDIS_HOST};
  const std::string port{std::to_string(Global::REDIS_PORT)};

  tcp::resolver resolver{io};
  const auto endpoints = resolver.resolve(host, port);

  tcp::socket socket{io};
  asio::connect(socket, endpoints);

  std::cout << "Connected to " << host << ":" << port << "\n";

  std::string line;
  std::cout << host << ":" << port << "> ";
  while (std::getline(std::cin, line)) {
    if (line.empty()) {
      std::cout << host << ":" << port << "> ";
      continue;
    }

    const std::string request = line + "\n";
    asio::write(socket, asio::buffer(request));

    const std::string response = readResponse(socket);
    const auto value = RESP::deserialize(response);
    std::cout << (value ? (*value)->toString() : "(nil)") << "\n";
    // We must flush after each command
    std::cout << host << ":" << port << "> " << std::flush;
  }

  return 0;
}
