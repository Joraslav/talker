add_service(server
  MAIN ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  DOMAIN_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Server.cpp
  DEPS talker::common talker::logger boost::boost
)
