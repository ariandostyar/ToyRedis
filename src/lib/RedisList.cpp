#include "RedisList.h"

RedisList::RedisList(std::deque<RedisObjectPtr> elements)
    : elements_(std::move(elements)) {}

void RedisList::pushBack(RedisObjectPtr element) {
  elements_.push_back(std::move(element));
}

void RedisList::pushFront(RedisObjectPtr element) {
  elements_.push_front(std::move(element));
}

std::string RedisList::serialize() const {
  std::string result = "*" + std::to_string(elements_.size()) + std::string(CRLF);
  for (const auto& element : elements_) {
    result += element->serialize();
  }
  return result;
}

std::string RedisList::toString() const {
  std::string result;
  for (size_t i = 0; i < elements_.size(); ++i) {
    result += std::to_string(i + 1) + ") \"" + elements_[i]->toString() + "\"";
    if (i + 1 < elements_.size()) {
      result += "\n";
    }
  }
  return result;
}
