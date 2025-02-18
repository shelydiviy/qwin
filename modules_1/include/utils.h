#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <asio.hpp>

struct Config {
    std::string serverIp;
    int serverPortStart;
    int serverPortEnd;
    std::string masterServerIp;
    int masterServerPort;
    std::string serverName;
    std::string mapName;
    int players;
    int maxPlayers;
    int protocolVersion;
    std::vector<std::string> playersList; // Добавляем список игроков
};

// Логирование сообщений
void logMessage(const std::string& message, const std::string& level);

// Создание пакета S2M_PING
size_t createMasterServerPacket(uint8_t* buffer, size_t bufferSize,
                                const std::string& serverIp, int serverPort,
                                const std::string& serverName, const std::string& mapName,
                                int players, int maxPlayers, int protocolVersion);

// Создание пакета A2S_INFO
size_t createA2SInfoPacket(uint8_t* buffer, size_t bufferSize,
                          const std::string& serverName, const std::string& mapName,
                          int players, int maxPlayers, int protocolVersion);

// Создание пакета A2S_PLAYER
size_t createA2SPlayerPacket(uint8_t* buffer, size_t bufferSize,
                            const std::vector<std::string>& playersList);

#endif // UTILS_H
