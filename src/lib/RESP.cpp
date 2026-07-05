// https://redis.io/docs/latest/develop/reference/protocol-spec/#resp-protocol-description

#include "RESP.h"
#include "RESPType.h"

RESP::RESP() {}

RESP::~RESP() {}

std::string RESP::serialize(RESPType type, std::string_view value) {
  std::string op;

  switch (type) 
  {
    case RESPType::SimpleString:
      op = "+";
      break;
    case RESPType::Error:
      op = "-";
      break;  
    case RESPType::Integer:
      op = ":";
      break;
    default:
      return "UNKNOWN";
  }
  return op + std::string(value) + std::string(CRLF);
}

std::string RESP::deserialize(std::string_view data) {
  if (data.empty()) {
    return "";
  }

  const char firstByte = data[0];
  std::string_view payload = data.substr(1);
  if (payload.size() >= 2 && payload.substr(payload.size() - 2) == CRLF) {
    payload = payload.substr(0, payload.size() - 2);
  }

  switch (firstByte) {
    case '+':
    case '-':
    case ':':
      return std::string(payload);
    default:
      return "";
  }
}
