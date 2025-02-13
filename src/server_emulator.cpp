#include "server_emulator.h"
#include "utils.h"
#include <asio.hpp>
#include <thread> // Добавляем эту строку
#include <chrono>

// Реализация конструктора
ServerEmulator::ServerEmulator(const std::string& ip, int port)
    : ip(ip), port(port), bound(false), ioContext(), socket(ioContext, asio::ip::udp::endpoint(asio::ip::udp::v4(), port)) {}

void ServerEmulator::listenForConnections() {
    try {
        logMessage("Listening on " + ip + ":" + std::to_string(port), "server");

        // Запускаем цикл обработки событий
        ioContext.run();

        while (true) {
            sendMasterServerInfo();
            std::this_thread::sleep_for(std::chrono::seconds(60)); // Используем sleep_for
        }
    } catch (const std::exception& e) {
        logMessage("Error in server on port " + std::to_string(port) + ": " + std::string(e.what()), "error");
    }
}

bool ServerEmulator::isBound() const {
    return bound;
}

void ServerEmulator::sendMasterServerInfo() {
    std::vector<std::pair<std::string, int>> masterServers = {
        {"hl2master.steampowered.com", 27011},
        {"208.64.200.55", 27011}
    };

    for (const auto& [masterIp, masterPort] : masterServers) {
        sendToMasterServer(masterIp, masterPort);
    }
}

void ServerEmulator::sendToMasterServer(const std::string& masterIp, int masterPort) {
    logMessage("Sending info to Master Server at " + masterIp + ":" + std::to_string(masterPort), "server");

    try {
        asio::ip::udp::resolver resolver(ioContext);
        asio::ip::udp::endpoint endpoint = *resolver.resolve(masterIp, std::to_string(masterPort)).begin();

        struct MasterPacket {
            uint8_t header = 0x66; // Заголовок пакета
            uint8_t challenge = 0x00; // Challenge
            uint8_t protocol = 48; // Версия протокола
            uint8_t serverType = 'd'; // Тип сервера (d - dedicated/internet server)
            uint8_t platform = 'l'; // Платформа (l - Linux)
            char gameDir[56] = "cstrike"; // Название директории игры
            char description[64] = "CS 1.6 Dedicated Mirror Server | CSDM | Sentry+Laser"; // Описание сервера
            char mapName[16] = "de_dust2"; // Имя карты
            char gameName[16] = "Counter-Strike"; // Название игры
            uint8_t players = 0; // Количество игроков
            uint8_t maxPlayers = 32; // Максимальное количество игроков
            uint8_t bots = 0; // Количество ботов
            uint8_t dedicated = 'd'; // Специально посвящённый сервер
            uint8_t os = 'l'; // Операционная система (Linux)
            uint8_t passwordProtected = 0; // Защищён ли паролем
            uint8_t hasMods = 1; // Есть ли модификации (1 - да, 0 - нет)
        };

        MasterPacket packet;
        std::vector<uint8_t> buffer(sizeof(MasterPacket));
        memcpy(buffer.data(), &packet, sizeof(MasterPacket));

        socket.send_to(asio::buffer(buffer), endpoint);
        logMessage("Successfully sent info to Master Server", "server");
    } catch (const std::exception& e) {
        logMessage("Failed to send data to Master Server: " + std::string(e.what()), "error");
    }
}