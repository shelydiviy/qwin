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
    Config config = loadConfig(configPath); // Убедитесь, что функция loadConfig определена

    asio::io_context ioContext;

    std::vector<std::unique_ptr<ServerEmulator>> servers;
    for (int port = config.serverPortStart; port <= config.serverPortEnd; ++port) {
        try {
            logMessage("С
