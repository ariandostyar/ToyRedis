#pragma once

#include <string>
#include <string_view>

#include "RESPType.h"

class RESP {
  static constexpr std::string_view CRLF = "\r\n";

public:
  RESP();
  ~RESP();

  static std::string serialize(RESPType type, std::string_view value);
  static std::string deserialize(std::string_view data);
};
