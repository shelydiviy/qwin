# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(qwin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск библиотеки Boost
find_package(Boost REQUIRED COMPONENTS system filesystem)

# Добавляем пути к Asio (заголовочная-only библиотека)
set(ASIO_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/third_party/asio/include")
if(NOT EXISTS "${ASIO_INCLUDE_DIR}/asio.hpp")
    message(FATAL_ERROR "Asio not found in ${ASIO_INCLUDE_DIR}. Please clone it as a submodule.")
endif()

include_directories(include ${ASIO_INCLUDE_DIR})

# Создание исполняемого файла
add_executable(qwin
    src/main.cpp
    src/server_emulator.cpp
    src/udp_proxy.cpp
    src/utils.cpp
)

# Связывание с библиотеками
target_link_libraries(qwin PRIVATE Boost::system Boost::filesystem)
