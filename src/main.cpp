// src/main.cpp
#include "server_emulator.h"
#include "steam.h"
#include <thread>
#include <vector>
#include <asio.hpp>

void worker(asio::io_context& ioContext) {
    ioContext.run();
}

void initializeThreadPool(asio::io_context& ioContext, size_t numThreads) {
    std::vector<std::thread> threads;
    for (size_t i = 0; i < numThreads; ++i) {
        threads.emplace_back(worker, std::ref(ioContext));
    }

    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
}

int main() {
    try {
        asio::io_context ioContext;

        // Инициализация пула потоков
        size_t numThreads = std::thread::hardware_concurrency();
        if (numThreads == 0) {
            std::cerr << "Не удалось определить количество ядер процессора. Используется 1 поток." << std::endl;
            numThreads = 1;
        }
        initializeThreadPool(ioContext, numThreads);

        // Создание и запуск Steam модуля
        Steam steam(ioContext);
        steam.start();

        // Создание и запуск серверов
        ServerEmulator server;
        server.start();

        // Запускаем цикл обработки событий
        ioContext.run();
    } catch (std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }

    return 0;
}
