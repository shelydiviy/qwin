#include "server_emulator.h"
#include "udp_proxy.h"
#include "utils.h"
#include <iostream>
#include <thread>

int main() {
    try {
        // Загрузка конфигурации
        std::cout << "Loading configuration..." << std::endl;
        Config config = loadConfig("./config/config.json");
        std::cout << "Configuration loaded successfully: "
                  << config.serverIp << ":" << config.serverPortStart
                  << " -> " << config.remoteServerIp << ":" << config.remoteServerPort << std::endl;

        // Логирование начала работы системы
        logMessage("System started with configuration: " + config.serverIp + ":" + std::to_string(config.serverPortStart) +
                   " -> " + config.remoteServerIp + ":" + std::to_string(config.remoteServerPort), "system");

        // Создание и запуск сервера
        std::cout << "Creating server emulator on " << config.serverIp << ":" << config.serverPortStart << "..." << std::endl;
        ServerEmulator server(config.serverIp, config.serverPortStart);
        std::thread serverThread(&ServerEmulator::listenForConnections, &server);
        std::cout << "Server thread started successfully." << std::endl;

        // Создание и запуск UDP-прокси
        std::cout << "Starting UDP proxy from ports " << config.serverPortStart << " to " << config.serverPortEnd
                  << " and forwarding to " << config.remoteServerIp << ":" << config.remoteServerPort << "..." << std::endl;
        UdpProxy proxy(config.serverPortStart, config.serverPortEnd, config.remoteServerIp, config.remoteServerPort);
        proxy.startProxy();

        // Ожидание завершения работы сервера
        serverThread.join();
        std::cout << "Server thread joined." << std::endl;

        // Логирование остановки системы
        logMessage("System stopped.", "system");
    } catch (const std::exception& e) {
        // Логирование ошибки
        logMessage("Error: " + std::string(e.what()), "error");
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}