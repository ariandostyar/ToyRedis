#pragma once

#include <memory>
#include <string>
#include <string_view>

inline constexpr std::string_view CRLF = "\r\n";

// Component interface for the Composite pattern.
class RedisObject {
public:
  virtual ~RedisObject() = default;

  virtual std::string serialize() const = 0;
  virtual std::string toString() const = 0;
};

using RedisObjectPtr = std::shared_ptr<RedisObject>;
