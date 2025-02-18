#include "server_emulator.h"
#include "utils.h"

ServerEmulator::ServerEmulator(const std::string& ipAddrParam, int portNumParam, asio::io_context& ioContextParam, const Config& configParam)
    : ip(ipAddrParam), port(portNumParam), ioContext(ioContextParam), config(configParam) {
    try {
        socket = asio::ip::udp::socket(ioContext, asio::ip::udp::endpoint(asio::ip::udp::v4(), port));
        if (socket.is_open()) {
            logMessage("Сервер успешно создан и привязан к порту " + ip + ":" + std::to_string(port), "server");
            bound = true;
        } else {
            logMessage("Не удалось создать сервер на порту " + std::to_string(port), "error");
        }
    } catch (const std::exception& e) {
        logMessage("Ошибка при создании сервера: " + std::string(e.what()), "error");
    }
}

ServerEmulator::~ServerEmulator() {
    if (socket.is_open()) {
        socket.close();
        logMessage("Сервер на порту " + std::to_string(port) + " остановлен", "server");
    }
}

void ServerEmulator::listenForConnections() {
    if (!bound) {
        logMessage("Сервер не привязан к порту " + std::to_string(port), "error");
        return;
    }

    logMessage("Сервер слушает подключения на порту " + std::to_string(port), "system");

    auto threadFunc = [this]() {
        while (true) {
            sendMasterServerInfo();
            std::this_thread::sleep_for(std::chrono::minutes(1));
        }
    };

    std::thread(threadFunc).detach();
}

void ServerEmulator::sendMasterServerInfo() {
    try {
        asio::ip::udp::resolver resolver(ioContext);
        asio::ip::udp::resolver::query query(asio::ip::udp::v4(), config.masterServerIp, std::to_string(config.masterServerPort));
        asio::ip::udp::endpoint masterEndpoint = *resolver.resolve(query);

        uint8_t buffer[1400];
        size_t packetSize = createMasterServerPacket(buffer, sizeof(buffer), ip, port, config.serverName, config.mapName, config.players, config.maxPlayers, config.protocolVersion);

        socket.async_send_to(asio::buffer(buffer, packetSize), masterEndpoint,
                            [masterIp = config.masterServerIp](std::error_code ec, [[maybe_unused]] std::size_t bytesSent) {
                                if (ec) {
                                    logMessage("Ошибка отправки информации мастер-серверу " + masterIp + ": " + ec.message(), "error");
                                } else {
                                    logMessage("Информация успешно отправлена мастер-серверу " + masterIp, "server");
                                }
                            });
    } catch (const std::exception& e) {
        logMessage("Ошибка при отправке информации мастер-серверу: " + std::string(e.what()), "error");
    }
}
