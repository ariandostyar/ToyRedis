#pragma once

#include "RedisObject.h"

#include <string>

class RedisError : public RedisObject {
public:
  explicit RedisError(std::string message) : message_(std::move(message)) {}

  const std::string& message() const { return message_; }

  std::string serialize() const override;
  std::string toString() const override;

private:
  std::string message_;
};
