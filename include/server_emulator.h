#ifndef SERVER_EMULATOR_H
#define SERVER_EMULATOR_H

#include <string>
#include <asio.hpp>
#include "utils.h" // Для Config

class ServerEmulator {
public:
    ServerEmulator(const std::string& ipAddr, int portNum, const Config& configData);
    void listenForConnections();
    bool isBound() const;

private:
    std::string ip;
    asio::io_context& ioContext;
    int port;
    bool bound = false;
    Config config; // Храним конфигурацию

    asio::io_context ioContext;
    asio::ip::udp::socket socket;

    void sendMasterServerInfo();
    void sendToMasterServer(const std::string& masterIp, int masterPort);
};

#endif // SERVER_EMULATOR_H
