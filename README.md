# ToyRedis

An educational in-memory key-value store inspired by [Redis](https://redis.io). Written in C++23, it speaks a subset of the [RESP](https://redis.io/docs/latest/develop/reference/protocol-spec/) protocol over TCP and focuses on clear object-oriented design: a Composite-pattern value hierarchy, an abstract base for RESP types, and a table-driven command registry.

## Demo

![ToyRedis demo](demog.gif)

The clip walks through the most interesting commands in the registry:

```
PING
PING hello
SET mykey hello
GET mykey
DEL mykey
GET mykey
FLUSHALL
```

| Command | What it does |
|---------|----------------|
| `PING [msg]` | Health check — replies `PONG`, or echoes an optional message |
| `SET key value` | Store a string value |
| `GET key` | Fetch a value (or null if missing) |
| `DEL key` | Delete a key; returns how many keys were removed |
| `FLUSHALL` | Wipe the entire in-memory store |

## Design choices

### Abstract `RedisObject`

All RESP values inherit from an abstract `RedisObject` with pure virtual `serialize()` (wire format) and `toString()` (human-readable). A virtual destructor keeps polymorphic deletion safe. Concrete types include leaves (`RedisString`, `RedisInteger`, `RedisNull`, `RedisError`) and composites (`RedisList`, `RedisSet`).

### Composite pattern

`RedisObject` is the Component. Leaf types serialize themselves; `RedisList` and `RedisSet` hold child `RedisObject`s and recurse into them when serializing.

### `shared_ptr` ownership (`RedisObjectPtr`)

Values are held as `std::shared_ptr<RedisObject>` (`RedisObjectPtr`), not `unique_ptr`. The store is an `unordered_map` of these pointers; lists and sets nest the same type. Shared ownership fits a polymorphic value graph where the same object type flows through the store, command handlers, and the RESP deserializer without ownership hand-offs or slicing. `unique_ptr` would force awkward moves every time a value is looked up or nested.

### Command registry

Commands live in a static map of name → `{minArgs, maxArgs, handler}`. `executeCommand` validates arity and dispatches — adding a command is mostly registering another entry.

### Single-threaded async I/O (epoll via Boost.Asio)

The server runs one `io_context` thread and uses Asio’s reactor (`async_accept` / `async_read_until` / `async_write`). On Linux that reactor is backed by **epoll**, so many connections are multiplexed without a thread per client. Each client is a `Session` kept alive with `shared_ptr` / `enable_shared_from_this` for the duration of outstanding ops. Because all handlers run on the same thread, the shared `Redis` store needs no locks.

## Build & run

Requires CMake 3.16+, Clang++, and Boost (Asio).

```bash
mkdir -p build && cd build
cmake ..
cmake --build .
```

```bash
# Terminal 1
./toyredis-server

# Terminal 2
./toyredis-cli
```

Server listens on `127.0.0.1:6379`.
