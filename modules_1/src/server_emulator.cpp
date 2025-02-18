#include "server_emulator.h"
#include "utils.h"
#include <thread>
#include <chrono>

ServerEmulator::ServerEmulator(const std::string& ipAddrParam, int portNumParam, asio::io_context& ioContextParam, const Config& configParam)
    : ip(ipAddrParam), port(portNumParam), ioContext(ioContextParam), config(configParam) {
    try {
        // Создание сокета и привязка к порту
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
            // Обработка входящих пакетов
            uint8_t buffer[1400];
            asio::ip::udp::endpoint senderEndpoint;
            socket.async_receive_from(asio::buffer(buffer, sizeof(buffer)), senderEndpoint,
                                     [this, senderEndpoint](std::error_code ec, std::size_t bytesReceived) {
                                         if (!ec && bytesReceived > 0) {
                                             if (buffer[0] == '\xFF' && buffer[1] == '\xFF' && buffer[2] == '\xFF' && buffer[3] == '\xFF') {
                                                 if (buffer[4] == 'i') {
                                                     handleA2SInfoRequest(senderEndpoint);
                                                 } else if (buffer[4] == 'p') {
                                                     handleA2SPlayerRequest(senderEndpoint);
                                                 }
                                             }
                                         } else {
                                             logMessage("Ошибка приёма пакета: " + ec.message(), "error");
                                         }
                                     });

            // Отправка информации на мастер-сервер каждую минуту
            sendMasterServerInfo();
            std::this_thread::sleep_for(std::chrono::minutes(1));
        }
    };

    std::thread(threadFunc).detach();
}

void ServerEmulator::sendMasterServerInfo() {
    try {
        // Создание resolver'а для получения IP-адреса мастер-сервера
        asio::ip::udp::resolver resolver(ioContext);
        asio::ip::udp::resolver::query query(asio::ip::udp::v4(), config.masterServerIp, std::to_string(config.masterServerPort));
        asio::ip::udp::endpoint masterEndpoint = *resolver.resolve(query);

        // Создание пакета S2M_PING
        uint8_t buffer[1400];
        size_t packetSize = createMasterServerPacket(buffer, sizeof(buffer), ip, port, config.serverName, config.mapName, config.players, config.maxPlayers, config.protocolVersion);

        // Отправка пакета на мастер-сервер
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

void ServerEmulator::handleA2SInfoRequest(asio::ip::udp::endpoint senderEndpoint) {
    try {
        // Создание ответа A2S_INFO
        uint8_t buffer[1400];
        size_t packetSize = createA2SInfoPacket(buffer, sizeof(buffer), config.serverName, config.mapName, config.players, config.maxPlayers, config.protocolVersion);

        // Отправка ответа клиенту
        socket.async_send_to(asio::buffer(buffer, packetSize), senderEndpoint,
                             [](std::error_code ec, [[maybe_unused]] std::size_t bytesSent) {
                                 if (ec) {
                                     logMessage("Ошибка отправки ответа A2S_INFO: " + ec.message(), "error");
                                 }
                             });
    } catch (const std::exception& e) {
        logMessage("Ошибка обработки запроса A2S_INFO: " + std::string(e.what()), "error");
    }
}

void ServerEmulator::handleA2SPlayerRequest(asio::ip::udp::endpoint senderEndpoint) {
    try {
        // Создание ответа A2S_PLAYER
        uint8_t buffer[1400];
        size_t packetSize = createA2SPlayerPacket(buffer, sizeof(buffer), config.playersList);

        // Отправка ответа клиенту
        socket.async_send_to(asio::buffer(buffer, packetSize), senderEndpoint,
                             [](std::error_code ec, [[maybe_unused]] std::size_t bytesSent) {
                                 if (ec) {
                                     logMessage("Ошибка отправки ответа A2S_PLAYER: " + ec.message(), "error");
                                 }
                             });
    } catch (const std::exception& e) {
        logMessage("Ошибка обработки запроса A2S_PLAYER: " + std::string(e.what()), "error");
    }
}
