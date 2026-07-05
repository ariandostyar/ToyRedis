#pragma once

#include "RedisObject.h"

#include <string>

// Leaf node holding a single string value.
class RedisString : public RedisObject {
public:
  explicit RedisString(std::string value) : value_(std::move(value)) {}

  const std::string& value() const { return value_; }

  std::string serialize() const override;
  std::string toString() const override;

private:
  std::string value_;
};
