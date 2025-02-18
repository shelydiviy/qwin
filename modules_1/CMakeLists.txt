cmake_minimum_required(VERSION 3.10)

# Имя проекта
project(tiny-csgo-mirror-system)

# Установка C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

# Включение предупреждений
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -Wunused-parameter")

# Поиск Boost библиотеки
find_package(Boost REQUIRED COMPONENTS system)
include_directories(include)

# Добавление исполняемого файла
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/server_emulator.cpp
    src/utils.cpp
)

# Связывание с Boost
target_link_libraries(${PROJECT_NAME} Boost::system)

# Копирование конфигурационного файла
configure_file(config.json ${CMAKE_BINARY_DIR}/config.json COPYONLY)
