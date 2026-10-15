add_my_library(logger
  SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Log.cpp
  INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
  DEPS spdlog::spdlog
)
