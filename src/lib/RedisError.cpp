#include "RedisError.h"

std::string RedisError::serialize() const {
  return "-" + message_ + std::string(CRLF);
}

std::string RedisError::toString() const {
  return "(error) " + message_;
}
