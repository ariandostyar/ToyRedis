#pragma once

#include "RedisObject.h"

class RedisNull : public RedisObject {
public:
  std::string serialize() const override;
  std::string toString() const override;
};
