cmake_minimum_required(VERSION 3.13)

project(dns-server-in-cpp)

set(CMAKE_CXX_STANDARD 23) # Enable the C++23 standard

# Collect all source files
file(GLOB_RECURSE ALL_SOURCE_FILES src/*.cpp src/*.hpp)

# Collect all files except client.cpp for the server
list(REMOVE_ITEM ALL_SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/client.cpp)

# Create an executable for the server
add_executable(my_dns_server ${ALL_SOURCE_FILES})

# Create an executable for the client
add_executable(my_dns_client src/client.cpp)
