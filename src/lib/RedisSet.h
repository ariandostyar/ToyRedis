#pragma once

#include "RedisObject.h"

#include <vector>

// Composite node representing an unordered set of Redis objects.
class RedisSet : public RedisObject {
public:
  RedisSet() = default;
  explicit RedisSet(std::vector<RedisObjectPtr> elements);

  const std::vector<RedisObjectPtr>& elements() const { return elements_; }

  bool insert(RedisObjectPtr element);

  std::string serialize() const override;
  std::string toString() const override;

private:
  std::vector<RedisObjectPtr> elements_;
};
