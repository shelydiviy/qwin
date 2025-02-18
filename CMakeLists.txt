cmake_minimum_required(VERSION 3.10)

project(tiny-csgo-mirror-system)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS system)

include_directories(include)

add_executable(tiny-csgo-mirror-system
    src/main.cpp
    src/server_emulator.cpp
    src/udp_proxy.cpp
    src/utils.cpp
)

target_link_libraries(tiny-csgo-mirror-system PRIVATE Boost::system)