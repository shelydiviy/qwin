# Устанавливаем минимальную версию CMake
cmake_minimum_required(VERSION 3.10)

# Указываем имя проекта
project(tiny-csgo-mirror-system VERSION 1.0 LANGUAGES CXX)

# Устанавливаем стандарт C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Находим Boost.Asio (если используется Boost)
find_package(Boost REQUIRED COMPONENTS system)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

# Добавляем исполняемый файл
add_executable(tiny-csgo-mirror-system
    src/main.cpp
    src/server_emulator.cpp
    src/udp_proxy.cpp
    src/utils.cpp
)

# Указываем директории с заголовочными файлами
target_include_directories(tiny-csgo-mirror-system PRIVATE include)

# Связываем библиотеки
target_link_libraries(tiny-csgo-mirror-system PRIVATE Boost::system)

# Добавляем флаги компиляции для оптимизации и предупреждений
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(tiny-csgo-mirror-system PRIVATE
        -Wall
        -Wextra
        -Wshadow
        -O2
    )
endif()

# Опционально: установка выходного каталога для исполняемых файлов
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)