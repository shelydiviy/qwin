#include "server_emulator.h"
#include "udp_proxy.h"
#include "utils.h"
#include <iostream>
#include <thread>
#include <vector>
#include <filesystem>

int main(int argc, char* argv[]) {
    try {
        // Проверяем, указан ли путь к конфигурационному файлу
        if (argc < 2) {
            std::cerr << "Usage: " << argv[0] << " --config <path_to_config>" << std::endl;
            return 1;
        }

        // Извлекаем путь к конфигурационному файлу
        std::string configPath = "./config/config.json"; // По умолчанию
        for (int i = 1; i < argc; ++i) {
            if (std::string(argv[i]) == "--config" && i + 1 < argc) {
                configPath = argv[i + 1];
                break;
            }
        }

        // Проверяем существование файла
        if (!std::filesystem::exists(configPath)) {
            std::cerr << "Error: Config file not found: " << configPath << std::endl;
            return 1;
        }

        // Загрузка конфигурации
        std::cout << "Loading configuration from: " << configPath << std::endl;
        Config config = loadConfig(configPath);
        std::cout << "Configuration loaded successfully: "
                  << config.serverIp << ":" << config.serverPortStart
                  << " -> " << config.remoteServerIp << ":" << config.remoteServerPort << std::endl;

        logMessage("System started with configuration: " + config.serverIp + ":" + std::to_string(config.serverPortStart) +
                   " -> " + config.remoteServerIp + ":" + std::to_string(config.remoteServerPort), "system");

        std::vector<std::thread> serverThreads;

        // Создание серверов на разных портах
        for (int port = config.serverPortStart; port <= config.serverPortEnd; ++port) {
            try {
                logMessage("Creating server on port " + std::to_string(port), "system");
                ServerEmulator server(config.serverIp, port);

                // Проверяем успешность привязки
                if (!server.isBound()) {
                    logMessage("Failed to bind to port " + std::to_string(port), "error");
                    continue;
                }

                serverThreads.emplace_back(std::thread(&ServerEmulator::listenForConnections, &server));
                logMessage("Server thread started on port " + std::to_string(port), "server");
            } catch (const std::exception& e) {
                logMessage("Error creating server on port " + std::to_string(port) + ": " + std::string(e.what()), "error");
            }
        }

        // Создание UDP-прокси
        logMessage("Starting UDP Proxy from ports " + std::to_string(config.serverPortStart) + " to " +
                   std::to_string(config.serverPortEnd) + " and forwarding to " + config.remoteServerIp + ":" +
                   std::to_string(config.remoteServerPort), "proxy");

        UdpProxy proxy(config.serverPortStart, config.serverPortEnd, config.remoteServerIp, config.remoteServerPort);
        std::thread proxyThread(&UdpProxy::startProxy, &proxy);

        // Ожидание завершения всех потоков
        for (auto& thread : serverThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }

        proxyThread.join();

        logMessage("System stopped.", "system");
    } catch (const std::exception& e) {
        logMessage("Critical error: " + std::string(e.what()), "error");
        std::cerr << "Critical error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}