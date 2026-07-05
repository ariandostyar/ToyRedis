#include "Redis.h"
#include "RESP.h"

#include <string>
#include <vector>

const std::unordered_map<std::string, Redis::Command> Redis::commandRegistry = {
    {"SET",
     {2,
      2,
      [](Redis& redis, const std::vector<std::string>& args) {
        redis.set(args[0], args[1]);
        return "OK";
      }}},
    {"GET",
     {1,
      1,
      [](Redis& redis, const std::vector<std::string>& args) {
        return redis.get(args[0]).value_or("KEY NOT FOUND");
      }}},
    {"DEL",
     {1,
      1,
      [](Redis& redis, const std::vector<std::string>& args) {
        return redis.deleteKey(args[0]);
      }}},
};


std::string Redis::executeCommand(const std::vector<std::string>& args) {
  if (args.empty()) {
    return RESP::serialize(RESPType::Error, defaultError);
  }

  const auto it = commandRegistry.find(args[0]);
  if (it == commandRegistry.end()) {
    return RESP::serialize(RESPType::Error, commandNotFoundError);
  }

  const size_t argc = args.size() - 1;
  const Command& command = it->second;
  if (argc < command.minArgs || argc > command.maxArgs) {
    return RESP::serialize(RESPType::Error, invalidArgCountError);
  }

  const std::vector<std::string> commandArgs(args.begin() + 1, args.end());
  const std::string response = command.execute(*this, commandArgs);
  return RESP::serialize(RESPType::SimpleString, response);
}

void Redis::set(const std::string& key, const std::string& value) {
  store_[key] = value;
}

std::optional<std::string> Redis::get(const std::string& key) const {
  const auto it = store_.find(key);
  if (it == store_.end()) {
    return std::nullopt;
  }
  return it->second;
}

std::string Redis::deleteKey(const std::string& key) {
  if (store_.find(key) == store_.end()) {
    return "KEY NOT FOUND";
  }
  store_.erase(key);
  return "KEY DELETED";
}
