cmake_minimum_required(VERSION 3.26)
project(WSPP)

set(CMAKE_CXX_STANDARD 17)

#add_executable(WSPP examples/example_ping.cpp)
add_executable(WSPP examples/example_chat.cpp)

target_link_libraries(WSPP ws2_32)
target_link_libraries(WSPP Crypt32.lib)
