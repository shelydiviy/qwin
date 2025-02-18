#ifndef SERVER_EMULATOR_H
#define SERVER_EMULATOR_H

#include <string>
#include <asio.hpp>
#include "utils.h"

class ServerEmulator {
public:
    ServerEmulator(const std::string& ipAddrParam, int portNumParam, asio::io_context& ioContextParam, const Config& configParam);
    ~ServerEmulator();

    void listenForConnections();
    bool isBound() const;

private:
    asio::ip::udp::socket socket; // Сокет должен быть первым
    asio::io_context& ioContext;
    Config config;
    std::string ip;
    int port;
    bool bound = false;

    void handleA2SInfoRequest(const asio::ip::udp::endpoint& senderEndpoint);
    void handleA2SPlayerRequest(const asio::ip::udp::endpoint& senderEndpoint);
    void sendMasterServerInfo();
};

#endif // SERVER_EMULATOR_H
