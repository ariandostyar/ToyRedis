// https://redis.io/docs/latest/develop/reference/protocol-spec/#resp-protocol-description

#include "RESP.h"
#include <string>

RESP::RESP() {}

RESP::~RESP() {}

std::string RESP::serialize(Global::RESPType type, std::string_view value) {
  std::string op;

  switch (type) {
    case Global::RESPType::SimpleString:
      op = "+";
      break;
    case Global::RESPType::Error:
      op = "-";
      break;
    case Global::RESPType::Integer:
      op = ":";
      break;
    case Global::RESPType::Array:
      op = "*";
      break;
    case Global::RESPType::Set:
      op = "~";
      break;
    case Global::RESPType::Boolean:
      op = "#";
      break;
    case Global::RESPType::Null:
    default:
      return "$-1" + std::string(CRLF);
  }
  return op + std::string(value) + std::string(CRLF);
}

Global::RedisObject RESP::deserialize(std::string_view data) {
  return "";
}

std::string RESP::toString(const Global::RedisObject& value) {
  return "OK\r\n";
}