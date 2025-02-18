#include "server_emulator.h"
#include "utils.h"
#include <thread>
#include <chrono>

ServerEmulator::ServerEmulator(const std::string& ipAddrParam, int portNumParam, asio::io_context& ioContextParam, const Config& configParam)
    : ip(ipAddrParam), port(portNumParam), ioContext(ioContextParam), config(configParam) {

// Конструктор с переименованными параметрами
ServerEmulator::ServerEmulator(const std::string& ipAddr, int portNum, const Config& configData)
    : ip(ipAddr), port(portNum), bound(false), config(configData),
      socket(ioContext, asio::ip::udp::endpoint(asio::ip::udp::v4(), portNum)) {
    if (socket.is_open()) {
        logMessage("Сервер успешно создан и привязан к порту " + ip + ":" + std::to_string(port), "server");
        bound = true;
    } else {
        logMessage("Не удалось создать сервер на порту " + std::to_string(port), "error");
    }
}

bool ServerEmulator::isBound() const {
    return bound;
}

void ServerEmulator::listenForConnections() {
    try {
        logMessage("Сервер слушает порт " + ip + ":" + std::to_string(port), "server");

        // Запуск io_context в отдельном потоке
        std::thread ioThread([this]() {
            ioContext.run();
        });

        while (true) {
            sendMasterServerInfo();
            std::this_thread::sleep_for(std::chrono::seconds(60)); // Отправляем информацию каждые 60 секунд
        }

        ioThread.join();
    } catch (const std::exception& e) {
        logMessage("Ошибка в работе сервера на порту " + std::to_string(port) + ": " + std::string(e.what()), "error");
    }
}

void ServerEmulator::sendMasterServerInfo() {
    std::vector<std::pair<std::string, int>> masterServers = {
        {"hl1master.steampowered.com", 27010}, // Адрес Master Server для GoldSrc
        {"208.64.200.55", 27010}             // Дополнительный адрес Master Server
    };

    for (const auto& [masterIp, masterPort] : masterServers) {
        sendToMasterServer(masterIp, masterPort);
    }
}

void ServerEmulator::sendToMasterServer(const std::string& masterIp, int masterPort) {
    logMessage("Отправка информации на Master Server: " + masterIp + ":" + std::to_string(masterPort), "server");

    try {
        struct MasterPacket {
            uint8_t header = 0x66; // Заголовок пакета
            uint8_t challenge = 0x00; // Challenge
            uint8_t protocol = 48; // Версия протокола GoldSrc
            uint8_t serverType = 'd'; // Тип сервера (dedicated/internet server)
            uint8_t platform = 'l'; // Платформа (Linux)
            char gameDir[56] = "cstrike"; // Название директории игры
            char description[64] = "CS 1.6 Зеркальный Сервер | CSDM | Sentry+Laser"; // Описание сервера
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

        asio::ip::udp::resolver resolver(ioContext); // Используем ioContext
        asio::ip::udp::endpoint endpoint = *resolver.resolve(masterIp, std::to_string(masterPort)).begin();

        socket.send_to(asio::buffer(buffer), endpoint);
        logMessage("Информация успешно отправлена на Master Server: " + masterIp + ":" + std::to_string(masterPort), "server");
    } catch (const std::exception& e) {
        logMessage("Не удалось отправить данные на Master Server: " + masterIp + ":" + std::to_string(masterPort) +
                   ". Ошибка: " + std::string(e.what()), "error");
    }
}
