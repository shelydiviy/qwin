#ifndef SERVER_EMULATOR_H
#define SERVER_EMULATOR_H

#include <string>
#include <asio.hpp>
#include "utils.h" // Подключаем utils.h для Config

class ServerEmulator {
public:
    ServerEmulator(const std::string& ipParam, int portParam, const Config& configParam); // Конструктор с Config
    void listenForConnections();
    bool isBound() const;

private:
    std::string ip;
    int port;
    bool bound = false;
    Config config; // Храним конфигурацию

    asio::io_context ioContext;
    asio::ip::udp::socket socket;

    void sendMasterServerInfo();
    void sendToMasterServer(const std::string& masterIp, int masterPort);
};

#endif // SERVER_EMULATOR_H