cmake_minimum_required(VERSION 3.16)
project(uchat2 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(client)
include_directories(server)

add_executable(uchat2
        server/main.c)
