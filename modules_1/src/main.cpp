#include "server_emulator.h"
#include "utils.h"
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

// Загрузка конфигурации
Config loadConfig(const std::string& configPath) {
    return loadConfig(configPath);
}

int main(int argc, char* argv[]) {
    try {
        if (argc != 3 || std::string(argv[1]) != "--config") {
            logMessage("Ошибка: неверные параметры командной строки", "error");
            std::cerr << "Использование: " << argv[0] << " --config <путь_к_файлу>" << std::endl;
            return 1;
        }

        // Загружаем конфигурацию
        std::string configPath = argv[2];
        Config config = loadConfig(configPath);

        // Создаем общий io_context
        asio::io_context ioContext;

        // Создаем серверы
        std::vector<std::unique_ptr<ServerEmulator>> servers;
        for (int port = config.serverPortStart; port <= config.serverPortEnd; ++port) {
            try {
                logMessage("Создание сервера на порту " + std::to_string(port), "system");
                servers.emplace_back(std::make_unique<ServerEmulator>(config.serverIp, port, ioContext, config));

                if (!servers.back()->isBound()) {
                    logMessage("Не удалось привязаться к порту " + std::to_string(port), "error");
                } else {
                    logMessage("Сервер успешно запущен на порту " + std::to_string(port), "server");
                }
            } catch (const std::exception& e) {
                logMessage("Ошибка создания сервера на порту " + std::to_string(port) + ": " + std::string(e.what()), "error");
            }
        }

        // Запускаем io_context
        logMessage("Система работает. Для завершения нажмите Ctrl+C", "system");
        ioContext.run();

    } catch (const std::exception& e) {
        logMessage("Критическая ошибка: " + std::string(e.what()), "error");
        std::cerr << "Критическая ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
