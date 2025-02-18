# Указываем минимальную версию CMake
cmake_minimum_required(VERSION 3.10)

# Имя проекта
project(tiny-csgo-mirror-system)

# Устанавливаем стандарт C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

# Настройка компилятора для отображения предупреждений
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -Wunused-parameter")

# Поиск Boost библиотеки
find_package(Boost REQUIRED COMPONENTS system)
include_directories(include)

# Добавляем исполняемый файл
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/server_emulator.cpp
    src/utils.cpp
)

# Связываем Boost с проектом
target_link_libraries(${PROJECT_NAME} Boost::system)

# Указываем путь к конфигурационному файлу
configure_file(config.json ${CMAKE_BINARY_DIR}/config.json COPYONLY)
