#pragma once

#include <cstdint>
#include <deque>
#include <string>
#include <string_view>
#include <unordered_set>
#include <variant>

namespace Global {
  inline constexpr uint16_t REDIS_PORT = 6379;
  inline constexpr std::string_view REDIS_HOST = "127.0.0.1";

  enum class RESPType {
    Error,
    SimpleString,
    Boolean,
    Integer,
    Array,
    Set,
    Null
  };

  using RedisObject = std::variant<
    std::string,
    std::deque<std::string>,
    std::unordered_set<std::string>>;
}
