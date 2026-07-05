add_library(toyredis STATIC
  RESP.cpp
)

target_include_directories(toyredis
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(toyredis
  PUBLIC
    Boost::headers
)
