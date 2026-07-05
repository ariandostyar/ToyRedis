
add_executable(toyredis-server main.cpp Redis.cpp)
target_link_libraries(toyredis-server PRIVATE toyredis)
