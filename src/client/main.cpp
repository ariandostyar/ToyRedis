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
  std::cout << "> ";
  while (std::getline(std::cin, line)) {
    if (line.empty()) {
      std::cout << "> ";
      continue;
    }

    asio::write(socket, asio::buffer(line + "\n"));

    const std::string response = readResponse(socket);
    std::cout << RESP::deserialize(response) << "\n";
    std::cout << "> ";
  }

  return 0;
}
