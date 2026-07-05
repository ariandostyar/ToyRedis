#pragma once

#include <cstdint>
#include <string_view>

namespace Globals {
  inline constexpr uint16_t REDIS_PORT = 6379;
  inline constexpr std::string_view REDIS_HOST = "127.0.0.1";
}