# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(qwin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск библиотеки Boost
find_package(Boost REQUIRED COMPONENTS system filesystem)

# Поиск библиотеки Asio
find_package(Asio REQUIRED)

# Добавление директории include для заголовочных файлов
include_directories(include)

# Добавляем пути к Asio, если используется заголовочная-only версия
if(NOT ASIO_FOUND)
    include_directories(third_party/asio/include)
endif()

# Создание исполняемого файла
add_executable(qwin
    src/main.cpp
    src/server_emulator.cpp
    src/udp_proxy.cpp
    src/utils.cpp
)

# Связывание с библиотеками
target_link_libraries(qwin PRIVATE Boost::system Boost::filesystem Asio::asio)
