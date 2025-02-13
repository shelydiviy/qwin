#include "server_emulator.h"
#include "utils.h"
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdlib>

// Реализация конструктора
ServerEmulator::ServerEmulator(const std::string& ip, int port) : ip(ip), port(port), bound(false) {}

// Структура данных для пакета Master Server
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

void ServerEmulator::sendToMasterServer(const std::string& masterIp, int masterPort) {
    logMessage("Sending info to Master Server at " + masterIp + ":" + std::to_string(masterPort), "server");

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        logMessage("Failed to create socket for Master Server", "error");
        return;
    }

    sockaddr_in masterAddr {};
    masterAddr.sin_family = AF_INET;
    masterAddr.sin_port = htons(masterPort);
    inet_pton(AF_INET, masterIp.c_str(), &(masterAddr.sin_addr));

    MasterPacket packet;
    std::vector<uint8_t> buffer(sizeof(MasterPacket));
    memcpy(buffer.data(), &packet, sizeof(MasterPacket));

    ssize_t bytesSent = sendto(sockfd, buffer.data(), buffer.size(), 0, (sockaddr*)&masterAddr, sizeof(masterAddr));
    if (bytesSent < 0) {
        logMessage("Failed to send data to Master Server", "error");
    } else {
        logMessage("Successfully sent info to Master Server", "server");
    }

    close(sockfd);
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

void ServerEmulator::listenForConnections() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        logMessage("Failed to create socket for port " + std::to_string(port), "error");
        return;
    }

    sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(sockfd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        logMessage("Failed to bind to port " + std::to_string(port), "error");
        close(sockfd);
        return;
    }

    bound = true; // Устанавливаем флаг привязки
    logMessage("Successfully bound to port " + std::to_string(port), "server");

    while (true) {
        sendMasterServerInfo();
        sleep(60); // Отправляем каждые 60 секунд
    }
}

bool ServerEmulator::isBound() const {
    return bound; // Возвращаем статус привязки
}