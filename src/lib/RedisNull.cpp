#include "RedisNull.h"

std::string RedisNull::serialize() const {
  return "$-1" + std::string(CRLF);
}

std::string RedisNull::toString() const {
  return "(nil)";
}
