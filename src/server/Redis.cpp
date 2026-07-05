#include "Redis.h"
#include "RESP.h"

#include <string>
#include <vector>

const std::unordered_map<std::string, Redis::Command> Redis::commandRegistry = {
  {
    "PING",
    {
      0,
      1,
      [](Redis& redis, const std::vector<std::string>& args)
      {
        std::string response;
        if (args.empty()) {
          response = redis.ping(); 
        } else {
          response = redis.ping(args[0]);
        }
        return RESP::serialize(Global::RESPType::SimpleString, response);
      }
    }
  },
  {
    "SET",
    {
      2,
      2,
      [](Redis& redis, const std::vector<std::string>& args) 
      {
        redis.set(args[0], args[1]);
        return RESP::serialize(Global::RESPType::SimpleString, "OK");
      }
    }
  },
  {
    "GET",
    {
      1,
      1,
      [](Redis& redis, const std::vector<std::string>& args) 
      {
        const auto value = redis.get(args[0]);
        return value ? RESP::serializeObject(*value) : RESP::serialize(Global::RESPType::Null, "");
      }
    }
  },
  {
    "DEL",
    {
      1,
      1,
      [](Redis& redis, const std::vector<std::string>& args) 
      {
        auto deletedCount = redis.deleteKey(args[0]);
        return RESP::serialize(Global::RESPType::Integer, deletedCount);
      }
    }
  },
};


std::string Redis::executeCommand(const std::vector<std::string>& args) {
  if (args.empty()) {
    return RESP::serialize(Global::RESPType::Error, defaultError);
  }

  const auto it = commandRegistry.find(args[0]);
  if (it == commandRegistry.end()) {
    return RESP::serialize(Global::RESPType::Error, commandNotFoundError);
  }

  const size_t argc = args.size() - 1;
  const Command& command = it->second;
  if (argc < command.minArgs || argc > command.maxArgs) {
    return RESP::serialize(Global::RESPType::Error, invalidArgCountError);
  }

  const std::vector<std::string> commandArgs(args.begin() + 1, args.end());
  return command.execute(*this, commandArgs);
}

void Redis::set(const std::string& key, Global::RedisObject value) {
  store_[key] = std::move(value);
}

std::optional<Global::RedisObject> Redis::get(const std::string& key) const {
  const auto it = store_.find(key);
  if (it == store_.end()) {
    return std::nullopt;
  }
  return it->second;
}

std::string Redis::deleteKey(const std::string& key) {
  if (store_.find(key) == store_.end()) {
    return "0";
  }
  store_.erase(key);
  return "1";
}

std::string Redis::ping(const std::string& customMessage) const {
  if (customMessage.empty()) {
      return "PONG";
  }
  return customMessage;
}
