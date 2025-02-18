#ifndef SERVER_EMULATOR_H
#define SERVER_EMULATOR_H

#include <string>
#include <asio.hpp>
#include "utils.h" // Добавляем utils.h

class ServerEmulator {
public:
    ServerEmulator(const std::string& ipAddrParam, int portNumParam, asio::io_context& ioContextParam, const Config& configParam);
    ~ServerEmulator();

    void listenForConnections();
    bool isBound() const;

private:
    asio::ip::udp::socket socket; // Сокет должен быть инициализирован раньше
    asio::io_context& ioContext;
    Config config; // Конфигурация сервера
    std::string ip;
    int port;
    bool bound = false;

    void sendMasterServerInfo();
};

#endif // SERVER_EMULATOR_H
