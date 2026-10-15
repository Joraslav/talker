add_service(client
  MAIN ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  DOMAIN_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/Client.cpp
  DEPS talker::common talker::logger boost::boost
)
