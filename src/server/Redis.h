#pragma once

#include "Global.h"

#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

class Redis {
public:
  void set(const std::string& key, Global::RedisObject value);
  std::optional<Global::RedisObject> get(const std::string& key) const;
  std::string deleteKey(const std::string& key);
  std::string ping(const std::string& customMessage = "") const;
  std::string executeCommand(const std::vector<std::string>& args);

private:
  static constexpr std::string_view defaultError = "ERR empty command";
  static constexpr std::string_view invalidArgCountError = "ERR wrong number of arguments";
  static constexpr std::string_view commandNotFoundError = "ERR unknown command";

  struct Command {
    size_t minArgs;
    size_t maxArgs;
    std::function<std::string(Redis&, const std::vector<std::string>&)> execute;
  };

  static const std::unordered_map<std::string, Command> commandRegistry;

  std::unordered_map<std::string, Global::RedisObject> store_;
};
