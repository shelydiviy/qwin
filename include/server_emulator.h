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
    std::string ip;
    int port;
    bool bound = false;
    Config config;
    asio::io_context& ioContext;
    asio::ip::udp::socket socket;

    void sendMasterServerInfo();
};

#endif // SERVER_EMULATOR_H
