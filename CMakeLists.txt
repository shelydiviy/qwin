# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(qwin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск библиотеки Boost
find_package(Boost REQUIRED COMPONENTS system filesystem)

# Добавление директории include для заголовочных файлов
include_directories(include)

# Создание исполняемого файла
add_executable(qwin
    src/main.cpp
    src/server_emulator.cpp
    src/udp_proxy.cpp
    src/utils.cpp
    src/steam.cpp  # Добавляем новый файл steam.cpp, если он существует
)

# Связывание с библиотекой Boost
target_link_libraries(qwin PRIVATE Boost::system Boost::filesystem)

# Если используются дополнительные зависимости (например, Asio), их нужно указать здесь
# find_package(Asio REQUIRED)
# target_link_libraries(qwin PRIVATE Asio::asio)
