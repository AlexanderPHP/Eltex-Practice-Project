cmake_minimum_required(VERSION 3.8)
project(Multicast)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic -O0 -g")

add_executable(server server.c)
add_executable(client client.c)