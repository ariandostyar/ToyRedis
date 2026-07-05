#include "RedisInteger.h"

std::string RedisInteger::serialize() const {
  return ":" + value_ + std::string(CRLF);
}

std::string RedisInteger::toString() const {
  return value_;
}
