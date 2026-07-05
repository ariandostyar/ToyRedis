#pragma once

#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

class Redis {
public:
  void set(const std::string& key, const std::string& value);
  std::optional<std::string> get(const std::string& key) const;
  std::string deleteKey(const std::string& key);
  std::string executeCommand(const std::vector<std::string>& args);

private:
  struct Command {
    size_t minArgs;
    size_t maxArgs;
    std::function<std::string(Redis&, const std::vector<std::string>&)> execute;
  };

  static const std::unordered_map<std::string, Command> commandRegistry;

  static constexpr std::string_view defaultError = "ERR empty command";
  static constexpr std::string_view invalidArgCountError =
      "ERR wrong number of arguments";
  static constexpr std::string_view commandNotFoundError = "ERR unknown command";

  std::unordered_map<std::string, std::string> store_;
};
