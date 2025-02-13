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
            std::cerr << "Usage: " << argv[0] << " --config <path_to_config>" << std::endl;
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

        logMessage("System started with configuration: " + config.serverIp + ":" + std::to_string(config.serverPortStart) +
                   " -> " + config.remoteServerIp + ":" + std::to_string(config.remoteServerPort), "system");

        std::vector<std::thread> serverThreads;

        for (int port = config.serverPortStart; port <= config.serverPortEnd; ++port) {
            try {
                logMessage("Creating server on port " + std::to_string(port), "system");
                ServerEmulator server(config.serverIp, port, config); // Передаём конфигурацию

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

        UdpProxy proxy(config.serverPortStart, config.serverPortEnd, config.remoteServerIp, config.remoteServerPort, config);
        std::thread proxyThread(&UdpProxy::startProxy, &proxy);

        while (true) {
            logMessage("System statistics: Total connections = " + std::to_string(totalConnections.load()) +
                       ", Blocked connections = " + std::to_string(blockedConnections.load()), "system");
            std::this_thread::sleep_for(std::chrono::minutes(1));
        }

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