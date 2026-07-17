#pragma once

#include "Redis.h"

#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

// One TCP client. Lifetime is tied to outstanding async ops via shared_from_this.
class Session : public std::enable_shared_from_this<Session> {
public:
  Session(tcp::socket socket, Redis& redis)
      : socket_(std::move(socket)), redis_(redis) {}

  void start() {
    std::cout << "New connection\n";
    doRead();
  }

private:
  void doRead() {
    auto self = shared_from_this();
    asio::async_read_until(
        socket_, buffer_, '\n',
        [this, self](const boost::system::error_code& error, std::size_t /*n*/) {
          if (error) {
            if (error != asio::error::eof) {
              std::cerr << "Read error: " << error.message() << "\n";
            } else {
              std::cout << "Connection dropped\n";
            }
            return;
          }

          std::istream input(&buffer_);
          std::string line;
          std::getline(input, line);

          const auto args = parseCommand(line);
          if (args.empty()) {
            doRead();
            return;
          }

          reply_ = redis_.executeCommand(args);
          doWrite();
        });
  }

  void doWrite() {
    auto self = shared_from_this();
    asio::async_write(
        socket_, asio::buffer(reply_),
        [this, self](const boost::system::error_code& error, std::size_t) {
          if (error) {
            std::cerr << "Write error: " << error.message() << "\n";
            return;
          }
          doRead();
        });
  }

  static std::vector<std::string> parseCommand(const std::string& line) {
    std::vector<std::string> args;
    std::istringstream input(line);
    std::string token;
    while (input >> token) {
      args.push_back(token);
    }
    return args;
  }

  tcp::socket socket_;
  asio::streambuf buffer_;
  Redis& redis_;
  std::string reply_;
};
