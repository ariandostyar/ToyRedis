#include "RedisString.h"

std::string RedisString::serialize() const {
  return "+" + value_ + std::string(CRLF);
}

std::string RedisString::toString() const {
  return value_;
}
