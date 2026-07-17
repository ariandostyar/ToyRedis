#include "Global.h"
#include "Redis.h"
#include "Session.h"

#include <boost/asio.hpp>
#include <iostream>
#include <memory>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

class Server {
public:
  Server(asio::io_context& io, std::uint16_t port)
      : acceptor_(io, tcp::endpoint{tcp::v4(), port}) {
    doAccept();
  }

private:
  void doAccept() {
    acceptor_.async_accept(
        [this](const boost::system::error_code& error, tcp::socket socket) {
          if (!error) {
            std::make_shared<Session>(std::move(socket), redis_)->start();
          } else {
            std::cerr << "Accept error: " << error.message() << "\n";
          }
          doAccept();
        });
  }

  tcp::acceptor acceptor_;
  Redis redis_;
};

int main() {
  try {
    asio::io_context io;
    Server server{io, Global::REDIS_PORT};

    std::cout << "ToyRedis listening on " << Global::REDIS_HOST << ":"
              << Global::REDIS_PORT << " (async / epoll)\n";

    io.run();
  } catch (const std::exception& error) {
    std::cerr << "Server error: " << error.what() << "\n";
    return 1;
  }

  return 0;
}
