#include "utils.h"
#include <cstring>

size_t createMasterServerPacket(uint8_t* buffer, size_t bufferSize, const std::string& serverIp, int serverPort,
                               const std::string& serverName, const std::string& mapName, int players, int maxPlayers, int protocolVersion) {
    // Формируем пакет S2M_PING
    if (bufferSize < 20) return 0; // Минимальный размер пакета
