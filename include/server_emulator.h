#ifndef SERVER_EMULATOR_H
#define SERVER_EMULATOR_H

#include <string>
#include <asio.hpp>
#include <thread>
#include <chrono>

class ServerEmulator {
public:
    ServerEmulator(const std::string& ip, int port);
    void listenForConnections();
    bool isBound() const;

private:
    std::string ip;
    int port;
    bool bound = false;

    asio::io_context ioContext; // Добавляем io_context
    asio::ip::udp::socket socket; // UDP-сокет

    void sendMasterServerInfo();
    void sendToMasterServer(const std::string& masterIp, int masterPort);
};

#endif // SERVER_EMULATOR_H