add_library(toyredis STATIC
  RESP.cpp
  Redis.cpp
  RedisString.cpp
  RedisError.cpp
  RedisInteger.cpp
  RedisNull.cpp
  RedisList.cpp
  RedisSet.cpp
)

target_include_directories(toyredis
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(toyredis
  PUBLIC
    Boost::headers
)
