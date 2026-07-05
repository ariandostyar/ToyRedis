// https://redis.io/docs/latest/develop/reference/protocol-spec/#resp-protocol-description

#include "RESP.h"

#include "RedisError.h"
#include "RedisInteger.h"
#include "RedisList.h"
#include "RedisNull.h"
#include "RedisSet.h"
#include "RedisString.h"

#include <string>

namespace {
  std::string_view readLine(std::string_view data, size_t& offset) {
    if (offset >= data.size()) {
      return "";
    }

    const size_t pos = data.find("\r\n", offset);
    if (pos == std::string_view::npos) {
      const std::string_view line = data.substr(offset);
      offset = data.size();
      return line;
    }

    const std::string_view line = data.substr(offset, pos - offset);
    offset = pos + 2;
    return line;
  }
}

std::optional<RedisObjectPtr> RESP::deserialize(std::string_view data) {
  size_t offset = 0;
  return deserializeInternal(data, offset);
}

std::optional<RedisObjectPtr> RESP::deserializeInternal(std::string_view data, size_t& offset) {
  if (offset >= data.size()) {
    return std::make_shared<RedisString>("");
  }

  const char prefix = data[offset++];

  if (prefix == '$') {
    const std::string_view line = readLine(data, offset);
    if (line == "-1") {
      return std::make_shared<RedisNull>();
    }
    return std::make_shared<RedisString>("(nil)");
  }

  const std::string_view line = readLine(data, offset);

  switch (prefix) {
    case '+':
      return std::make_shared<RedisString>(std::string(line));
    case '-':
      return std::make_shared<RedisError>(std::string(line));
    case ':':
      return std::make_shared<RedisInteger>(std::string(line));
    case '#':
      return std::make_shared<RedisString>(line == "t" ? "true" : "false");
    case '*': {
      const int count = std::stoi(std::string(line));
      if (count == -1) {
        return std::make_shared<RedisNull>();
      }

      std::deque<RedisObjectPtr> elements;
      for (int i = 0; i < count; ++i) {
        elements.push_back(*deserializeInternal(data, offset));
      }
      return std::make_shared<RedisList>(std::move(elements));
    }
    case '~': {
      const int count = std::stoi(std::string(line));
      std::vector<RedisObjectPtr> elements;
      elements.reserve(static_cast<size_t>(count));
      for (int i = 0; i < count; ++i) {
        elements.push_back(*deserializeInternal(data, offset));
      }
      return std::make_shared<RedisSet>(std::move(elements));
    }
    default:
      return std::make_shared<RedisString>("unknown response format");
  }
}
