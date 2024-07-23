cmake_minimum_required(VERSION 3.10)
project(example1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(fmt REQUIRED)
find_package(CURL REQUIRED)

add_executable(example1 ${CMAKE_SOURCE_DIR}/examples/example1.cpp)
target_link_libraries(example1 PRIVATE Threads::Threads OpenSSL::SSL OpenSSL::Crypto fmt::fmt CURL::libcurl)
