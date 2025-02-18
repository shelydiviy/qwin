#include "udp_proxy.h"
#include "utils.h"
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

Config loadConfig(const std::string& configPath) {
    return loadConfig(configPath);
}

class ThreadPool {
public:
    ThreadPool(std::size_t threadsCount) : ioContext(), workers(threadsCount) {
        for (std::size_t i = 0; i < threadsCount; ++i) {
            workers.emplace_back([this]() { ioContext.run(); });
        }
    }

    ~ThreadPool() {
        ioContext.stop();
        for (auto& worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }

    asio::io_context& getIoContext() { return ioContext; }

private:
    asio::io_context ioContext;
    std::vector<std::thread> workers;
};

int main(int argc, char* argv[]) {
    try {
        if (argc < 2) {
            logMessage("Ошибка: не указан путь к конфигурационному файлу", "error");
            std::cerr << "Использование: " << argv[0] << " --config <путь_к_файлу>" << std::endl;
            return 1;
        }

        // Загрузка конфигурации
        std::string configPath = argv[2];
        Config config = loadConfig(configPath);

        // Создание пула потоков
        ThreadPool pool(std::thread::hardware_concurrency());
        asio::io_context& ioContext = pool.getIoContext();

        // Создание серверов
        std::vector<std::unique_ptr<ServerEmulator>> servers;
        for (int portNum = config.serverPortStart; portNum <= config.serverPortEnd; ++portNum) {
            try {
                logMessage("Создание сервера на порту " + std::to_string(portNum), "system");
                servers.emplace_back(std::make_unique<ServerEmulator>(config.serverIp, portNum, ioContext, config));

                if (!servers.back()->isBound()) {
                    logMessage("Не удалось привязаться к порту " + std::to_string(portNum), "error");
                } else {
                    logMessage("Сервер запущен на порту " + std::to_string(portNum), "server");
                }
            } catch (const std::exception& e) {
                logMessage("Ошибка создания сервера на порту " + std::to_string(portNum) + ": " + std::string(e.what()), "error");
            }
        }

        // Создание UDP Proxy
        UdpProxy proxy(config.serverPortStart, config.serverPortEnd, config.remoteServerIp, config.remoteServerPort, ioContext, config);
        proxy.startProxy();

        // Ожидание завершения работы
        logMessage("Система работает. Для завершения нажмите Ctrl+C", "system");
        pool.getIoContext().run();

    } catch (const std::exception& e) {
        logMessage("Критическая ошибка: " + std::string(e.what()), "error");
        std::cerr << "Критическая ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
