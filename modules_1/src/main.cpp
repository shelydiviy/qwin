#include "server_emulator.h"
#include "utils.h"
#include <signal.h>
#include <memory>
#include <vector>

void handleSignal(int signal) {
    if (signal == SIGINT) {
        logMessage("Программа завершается по сигналу Ctrl+C", "system");
        exit(0);
    }
}

int main(int argc, char* argv[]) {
    signal(SIGINT, handleSignal); // Обработка Ctrl+C

    if (argc != 3 || std::string(argv[1]) != "--config") {
        logMessage("Ошибка: неверные параметры командной строки", "error");
        std::cerr << "Использование: " << argv[0] << " --config <путь_к_файлу>" << std::endl;
        return 1;
    }

    std::string configPath = argv[2];
    Config config = loadConfig(configPath);

    asio::io_context ioContext;

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

    logMessage("Система работает. Для завершения нажмите Ctrl+C", "system");
    ioContext.run();

    return 0;
}
