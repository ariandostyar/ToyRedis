#pragma once

#include <string>
#include <string_view>

#include "Global.h"

class RESP {
public:
  static constexpr std::string_view CRLF = "\r\n";

  RESP();
  ~RESP();

  static std::string serialize(Global::RESPType type, std::string_view value);
  static Global::RedisObject deserialize(std::string_view data);
  static std::string toString(const Global::RedisObject& value);
};
