cmake_minimum_required(VERSION 3.10)

project(tiny-csgo-mirror-system)

set(CMAKE_CXX_STANDARD 17)

add_executable(tiny-csgo-mirror-system
    src/main.cpp
    src/server_emulator.cpp
    src/udp_proxy.cpp
    src/utils.cpp
)

target_include_directories(tiny-csgo-mirror-system PRIVATE include)