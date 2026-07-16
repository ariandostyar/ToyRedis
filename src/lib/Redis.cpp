#include "Redis.h"
#include "RedisError.h"
#include "RedisInteger.h"
#include "RedisNull.h"
#include "RedisString.h"

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
        const std::string response = args.empty() ? redis.ping() : redis.ping(args[0]);
        return std::make_shared<RedisString>(response)->serialize();
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
        redis.set(args[0], std::make_shared<RedisString>(args[1]));
        return std::make_shared<RedisString>("OK")->serialize();
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
        return value ? (*value)->serialize() : std::make_shared<RedisNull>()->serialize();
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
        const auto deletedCount = redis.deleteKey(args[0]);
        return std::make_shared<RedisInteger>(deletedCount)->serialize();
      }
    }
  },
  {
    "FLUSHALL",
    {
      0,
      0,
      [](Redis& redis, const std::vector<std::string>& args)
      {
        redis.flushAll();
        return std::make_shared<RedisString>("OK")->serialize();
      }
    }
  },
};

std::string Redis::executeCommand(const std::vector<std::string>& args) {
  if (args.empty()) {
    return std::make_shared<RedisError>(std::string(defaultError))->serialize();
  }

  const auto it = commandRegistry.find(args[0]);
  if (it == commandRegistry.end()) {
    return std::make_shared<RedisError>(std::string(commandNotFoundError))->serialize();
  }

  const size_t argc = args.size() - 1;
  const Command& command = it->second;
  if (argc < command.minArgs || argc > command.maxArgs) {
    return std::make_shared<RedisError>(std::string(invalidArgCountError))->serialize();
  }

  const std::vector<std::string> commandArgs(args.begin() + 1, args.end());
  return command.execute(*this, commandArgs);
}

void Redis::set(const std::string& key, RedisObjectPtr value) {
  store_[key] = std::move(value);
}

std::optional<RedisObjectPtr> Redis::get(const std::string& key) const {
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

void Redis::flushAll() {
  store_.clear();
}
