#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <unordered_map>
#include <asio.hpp>

// Создание пакета для мастер-сервера
size_t createMasterServerPacket(uint8_t* buffer, size_t bufferSize, const std::string& serverIp, int serverPort,
                                const std::string& serverName, const std::string& mapName, int players, int maxPlayers, int protocolVersion);

// Создание пакета A2S_INFO
size_t createA2SInfoPacket(uint8_t* buffer, size_t bufferSize, const std::string& serverName, const std::string& mapName,
                          int players, int maxPlayers, int protocolVersion);

// Создание пакета A2S_PLAYER
size_t createA2SPlayerPacket(uint8_t* buffer, size_t bufferSize, const std::vector<std::string>& playersList);

// Логирование сообщений
void logMessage(const std::string& message, const std::string& level);

#endif // UTILS_H
