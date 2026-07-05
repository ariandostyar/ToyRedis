#pragma once

#include "RedisObject.h"

#include <optional>
#include <string_view>

class RESP {
public:
  static std::optional<RedisObjectPtr> deserialize(std::string_view data);

private:
  static std::optional<RedisObjectPtr> deserializeInternal(std::string_view data, size_t& offset);
};
