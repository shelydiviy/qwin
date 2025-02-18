# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

project(qwin LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(qwin
    src/main.cpp
    src/server.cpp
    src/server.h
    src/steam.cpp
    src/steam.h
)

find_package(Boost REQUIRED COMPONENTS system filesystem)
target_link_libraries(qwin PRIVATE Boost::system Boost::filesystem)
