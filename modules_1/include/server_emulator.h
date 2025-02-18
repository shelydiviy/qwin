#ifndef SERVER_EMULATOR_H
#define SERVER_EMULATOR_H

#include <string>
#include <asio.hpp>
#include "utils.h"

class ServerEmulator {
public:
    // Конструктор
    ServerEmulator(const std::string& ipAddrParam, int portNumParam, asio::io_context& ioContextParam, const Config& configParam);

    // Деструктор
    ~ServerEmulator();

    // Метод для прослушивания подключений
    void listenForConnections();

    // Метод для отправки информации на мастер-сервер
    void sendMasterServerInfo();

    // Проверка, привязан ли сервер к порту
    bool isBound() const;

private:
    std::string ip;
    int port;
    bool bound = false;
    Config config;
    asio::io_context& ioContext;
    asio::ip::udp::socket socket;

    // Метод для создания и отправки пакета A2S_INFO
    void handleA2SInfoRequest(asio::ip::udp::endpoint senderEndpoint);

    // Метод для создания и отправки пакета A2S_PLAYER
    void handleA2SPlayerRequest(asio::ip::udp::endpoint senderEndpoint);
};

#endif // SERVER_EMULATOR_H
