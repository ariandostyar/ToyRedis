#pragma once

#include "RedisObject.h"

#include <deque>

// Composite node representing an ordered list of Redis objects.
class RedisList : public RedisObject {
public:
  RedisList() = default;
  explicit RedisList(std::deque<RedisObjectPtr> elements);

  const std::deque<RedisObjectPtr>& elements() const { return elements_; }

  void pushBack(RedisObjectPtr element);
  void pushFront(RedisObjectPtr element);

  std::string serialize() const override;
  std::string toString() const override;

private:
  std::deque<RedisObjectPtr> elements_;
};
