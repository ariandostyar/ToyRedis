add_executable(toyredis-server main.cpp)
target_include_directories(toyredis-server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(toyredis-server PRIVATE toyredis)
