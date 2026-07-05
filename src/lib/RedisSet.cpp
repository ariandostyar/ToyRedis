#include "RedisSet.h"

RedisSet::RedisSet(std::vector<RedisObjectPtr> elements)
    : elements_(std::move(elements)) {}

bool RedisSet::insert(RedisObjectPtr element) {
  const std::string key = element->toString();
  for (const auto& existing : elements_) {
    if (existing->toString() == key) {
      return false;
    }
  }
  elements_.push_back(std::move(element));
  return true;
}

std::string RedisSet::serialize() const {
  std::string result = "~" + std::to_string(elements_.size()) + std::string(CRLF);
  for (const auto& element : elements_) {
    result += element->serialize();
  }
  return result;
}

std::string RedisSet::toString() const {
  std::string result;
  for (size_t i = 0; i < elements_.size(); ++i) {
    result += std::to_string(i + 1) + ") \"" + elements_[i]->toString() + "\"";
    if (i + 1 < elements_.size()) {
      result += "\n";
    }
  }
  return result;
}
