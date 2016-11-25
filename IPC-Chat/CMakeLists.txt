cmake_minimum_required(VERSION 3.6)
project(IPC_Chat)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic -std=c11 -O0 -g")

set(SOURCE_FILES src/main.c)
add_executable(IPC_Chat ${SOURCE_FILES})