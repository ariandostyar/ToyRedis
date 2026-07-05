#pragma once

#include "RedisObject.h"

#include <string>

class RedisInteger : public RedisObject {
public:
  explicit RedisInteger(std::string value) : value_(std::move(value)) {}

  const std::string& value() const { return value_; }

  std::string serialize() const override;
  std::string toString() const override;

private:
  std::string value_;
};
