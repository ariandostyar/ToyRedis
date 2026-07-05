#include "Global.h"
#include "Redis.h"

#include <boost/asio.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

std::vector<std::string> parseCommand(const std::string& line) {
  std::vector<std::string> args;
  std::istringstream input(line);
  std::string token;
  while (input >> token) {
    args.push_back(token);
  }
  return args;
}

int main() {
  asio::io_context io;

  tcp::acceptor acceptor{io, tcp::endpoint{tcp::v4(), Global::REDIS_PORT}};
  Redis redis;

  std::cout << "ToyRedis listening on " << Global::REDIS_HOST << ":" << Global::REDIS_PORT << "\n";

  for (;;) {
    tcp::socket socket{io};
    acceptor.accept(socket);
    std::cout << "New connection\n";

    try {
      for (;;) {
        asio::streambuf buffer;
        asio::read_until(socket, buffer, "\n");
        std::istream input(&buffer);
        std::string line;
        std::getline(input, line);

        const auto args = parseCommand(line);
        if (args.empty()) {
          continue;
        }

        const std::string reply = redis.executeCommand(args);
        asio::write(socket, asio::buffer(reply));
      }
    } catch (const std::exception& error) {
      std::cerr << "Connection error: " << error.what() << "\n";
    }
  }
}
