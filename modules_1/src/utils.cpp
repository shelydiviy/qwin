#include "utils.h"

size_t createMasterServerPacket(uint8_t* buffer, size_t bufferSize, const std::string& serverIp, int serverPort,
                               const std::string& serverName, const std::string& mapName, int players, int maxPlayers, int protocolVersion) {
    if (bufferSize < 1400) return 0;

    // Пример формирования пакета S2M_PING
    const char header[] = "\xFF\xFF\xFF\xFFs";
    std::string payload = serverIp + ":" + std::to_string(serverPort);

    size_t totalSize = sizeof(header) - 1 + payload.size();
    if (totalSize > bufferSize) return 0;

    memcpy(buffer, header, sizeof(header) - 1);
    memcpy(buffer + sizeof(header) - 1, payload.c_str(), payload.size());

    return totalSize;
}
