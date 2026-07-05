cmake_minimum_required(VERSION 4.3)

set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

project(ToyRedis CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost CONFIG REQUIRED)

add_executable(toyredis src/main.cpp)
target_link_libraries(toyredis Boost::headers)
