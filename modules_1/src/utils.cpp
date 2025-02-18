#include "utils.h"

// Логирование сообщений
void logMessage(const std::string& message, const std::string& level) {
    std::cout << "[" << level << "] " << message << std::endl;
}

// Создание пакета S2M_PING
size_t createMasterServerPacket(uint8_t* buffer, size_t bufferSize,
                                const std::string& serverIp, int serverPort,
                                const std::string& serverName, const std::string& mapName,
                                int players, int maxPlayers, int protocolVersion) {
    if (bufferSize < 1400) return 0;

    std::string payload = serverIp + ":" + std::to_string(serverPort);

    size_t totalSize = payload.size();
    if (totalSize > bufferSize) return 0;

    memcpy(buffer, payload.c_str(), totalSize);

    return totalSize;
}

// Создание пакета A2S_INFO
size_t createA2SInfoPacket(uint8_t* buffer, size_t bufferSize,
                          const std::string& serverName, const std::string& mapName,
                          int players, int maxPlayers, int protocolVersion) {
    if (bufferSize < 1400) return 0;

    uint8_t header[] = { '\xFF', '\xFF', '\xFF', '\xFF', 'I' };
    std::string payload = serverName + "\0" + mapName + "\0" + std::to_string(players) + "\0" + std::to_string(maxPlayers) + "\0" + std::to_string(protocolVersion);

    size_t totalSize = sizeof(header) - 1 + payload.size();
    if (totalSize > bufferSize) return 0;

    memcpy(buffer, header, sizeof(header) - 1);
    memcpy(buffer + sizeof(header) - 1, payload.c_str(), payload.size());

    return totalSize;
}

// Создание пакета A2S_PLAYER
size_t createA2SPlayerPacket(uint8_t* buffer, size_t bufferSize,
                            const std::vector<std::string>& playersList) {
    if (bufferSize < 1400) return 0;

    uint8_t header[] = { '\xFF', '\xFF', '\xFF', '\xFF', 'P' };
    std::string payload;

    for (const auto& player : playersList) {
        payload += player + "\0";
    }

    size_t totalSize = sizeof(header) - 1 + payload.size();
    if (totalSize > bufferSize) return 0;

    memcpy(buffer, header, sizeof(header) - 1);
    memcpy(buffer + sizeof(header) - 1, payload.c_str(), payload.size());

    return totalSize;
}
