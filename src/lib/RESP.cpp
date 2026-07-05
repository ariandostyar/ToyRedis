// https://redis.io/docs/latest/develop/reference/protocol-spec/#resp-protocol-description

#include "RESP.h"
#include <string>

RESP::RESP() {}

RESP::~RESP() {}

std::string_view readLine(std::string_view data, size_t& offset) {
  if (offset >= data.size()) return "";
  
  size_t pos = data.find("\r\n", offset);
  if (pos == std::string_view::npos) {
      std::string_view line = data.substr(offset);
      offset = data.size();
      return line;
  }
  
  std::string_view line = data.substr(offset, pos - offset);
  offset = pos + 2; // skip past \r\n
  return line;
}

std::string RESP::serialize(Global::RESPType type, std::string_view value) {
  std::string op;

  switch (type) {
    case Global::RESPType::SimpleString:
      op = "+";
      break;
    case Global::RESPType::Error:
      op = "-";
      break;
    case Global::RESPType::Integer:
      op = ":";
      break;
    case Global::RESPType::Array:
      op = "*";
      break;
    case Global::RESPType::Set:
      op = "~";
      break;
    case Global::RESPType::Boolean:
      op = "#";
      break;
    case Global::RESPType::Null:
    default:
      return "$-1" + std::string(CRLF);
  }
  return op + std::string(value) + std::string(CRLF);
}

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
std::string RESP::serializeObject(const Global::RedisObject& value) {
  return std::visit(overloaded {
    [](const std::string& str) -> std::string {
      // Real Redis GET returns a Bulk String, but since your serialize handles SimpleString:
      return RESP::serialize(Global::RESPType::SimpleString, str);
    },
    [](const std::deque<std::string>& deque) -> std::string {
      // Format as a RESP Array (*<count>\r\n...)
      std::string result = "*" + std::to_string(deque.size()) + "\r\n";
      for (const auto& item : deque) {
        // Assuming your array holds elements that should be serialized as SimpleStrings
        result += RESP::serialize(Global::RESPType::SimpleString, item);
      }
      return result;
    },
    [](const std::unordered_set<std::string>& set) -> std::string {
      // Format as a RESP Set (~<count>\r\n...)
      std::string result = "~" + std::to_string(set.size()) + "\r\n";
      for (const auto& item : set) {
        result += RESP::serialize(Global::RESPType::SimpleString, item);
      }
      return result;
    }
  }, value);
}

Global::RedisObject RESP::deserialize(std::string_view data) {
  size_t offset = 0;
  return deserializeInternal(data, offset);
}

// Internal recursive parser to cleanly handle single elements or nested arrays
Global::RedisObject RESP::deserializeInternal(std::string_view data, size_t& offset) {
  if (offset >= data.size()) return std::string("");

  char prefix = data[offset++];

  std::string_view line = readLine(data, offset);

  switch (prefix) 
  {
    case '+':
      return std::string(line);
    case '-':
      return "(error) " + std::string(line);
    case ':':
      return "(integer) " + std::string(line);
    case '#':
      return (line == "t") ? std::string("true") : std::string("false");
    case '$': 
      return std::string("(nil)");
    case '*': { // Array (e.g., "*2\r\n:1\r\n+OK\r\n")
      int count = std::stoi(std::string(line));
      if (count == -1) return std::string("(nil)"); // Null Array
      
      std::deque<std::string> arrayResult;
      for (int i = 0; i < count; ++i) {
          // Recursively parse the next item and stringify it for the container
          auto obj = deserializeInternal(data, offset);
          arrayResult.push_back(RESP::toString(obj));
      }
      return arrayResult;
    }
    case '~': { // Set (e.g., "~2\r\n+foo\r\n+bar\r\n")
      int count = std::stoi(std::string(line));
      std::unordered_set<std::string> setResult;
      for (int i = 0; i < count; ++i) {
          auto obj = deserializeInternal(data, offset);
          setResult.insert(RESP::toString(obj));
      }
      return setResult;
    }

    default:
        return std::string("unknown response format");
  }
}

std::string RESP::toString(const Global::RedisObject& value) {
  return std::visit(overloaded {
    [](const std::string& str) -> std::string {
        return str;
    },
    [](const std::deque<std::string>& deque) -> std::string {
        std::string result;
        for (size_t i = 0; i < deque.size(); ++i) {
            result += std::to_string(i + 1) + ") \"" + deque[i] + "\"";
            if (i != deque.size() - 1) result += "\n";
        }
        return result;
    },
    [](const std::unordered_set<std::string>& set) -> std::string {
        std::string result;
        size_t i = 1;
        for (const auto& item : set) {
            result += std::to_string(i++) + ") \"" + item + "\"";
            if (i <= set.size()) result += "\n";
        }
        return result;
    }
  }, value);
}