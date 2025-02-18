#include "server_emulator.h"
#include "udp_proxy.h"
#include "utils.h"
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

int main(int argc, char* argv[]) {
    try {
        if (argc < 2) {
            std::cerr << "Использование: " << argv[0] << " --config <путь_к_конфигу>" << std::endl;
            return 1;
        }

        std::string configPath = "./config/config.json";
        for (int i = 1; i < argc; ++i) {
            if (std::string(argv[i]) == "--config" && i + 1 < argc) {
                configPath = argv[i + 1];
                break;
            }
        }

        Config config = loadConfig(configPath);

        logMessage("Система запущена с конфигурацией: " + config.serverIp + ":" + std::to_string(config.serverPortStart) +
                   " -> " + config.remoteServerIp + ":" + std::to_string(config.remoteServerPort), "system");

        std::vector<std::thread> serverThreads;

        for (int port = config.serverPortStart; port <= config.serverPortEnd; ++port) {
            try {
                logMessage("Создание сервера на порту " + std::to_string(port), "system");
                ServerEmulator server(config.serverIp, port, config);

                if (!server.isBound()) {
                    logMessage("Не удалось привязаться к порту " + std::to_string(port), "error");
                    continue;
                }

                serverThreads.emplace_back(std::thread(&ServerEmulator::listenForConnections, &server));
                logMessage("Сервер запущен на порту " + std::to_string(port), "server");
            } catch (const std::exception& e) {
                logMessage("Ошибка создания сервера на порту " + std::to_string(port) + ": " + std::string(e.what()), "error");
            }
        }

        UdpProxy proxy(config.serverPortStart, config.serverPortEnd, config.remoteServerIp, config.remoteServerPort, config);
        std::thread proxyThread(&UdpProxy::startProxy, &proxy);

        logMessage("Система работает. Для завершения нажмите Ctrl+C", "system");

        while (true) {
            logMessage("Статистика системы: Общее количество подключений = " + std::to_string(totalConnections.load()) +
                       ", Заблокированные подключения = " + std::to_string(blockedConnections.load()), "system");
            std::this_thread::sleep_for(std::chrono::minutes(1));
        }

        for (auto& thread : serverThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }

        proxyThread.join();

        logMessage("Система остановлена.", "system");
    } catch (const std::exception& e) {
        logMessage("Критическая ошибка: " + std::string(e.what()), "error");
        std::cerr << "Критическая ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}