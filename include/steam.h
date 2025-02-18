// src/steam.h
#ifndef STEAM_H
#define STEAM_H

#include <asio.hpp>
#include <vector>

struct ChallengePacket {
    uint8_t header = 0x69; // Заголовок пакета
    uint32_t challenge;   // Challenge-ключ
};

class Steam {
public:
    Steam(asio::io_context& ioContext);
    void start();
    void sendChallengeResponse(uint32_t challenge);
    void sendServerUpdate();

private:
    asio::ip::udp::socket socket;
    asio::ip::udp::endpoint masterEndpoint;

    void initializeMasterConnection();
};

#endif // STEAM_H
