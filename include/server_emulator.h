#ifndef SERVER_EMULATOR_H
#define SERVER_EMULATOR_H

#include <string>

class ServerEmulator {
public:
    ServerEmulator(const std::string& ip, int port);
    void sendMasterServerInfo();
    void listenForConnections();
    bool isBound() const; // Метод для проверки привязки

private:
    std::string ip;
    int port;
    bool bound = false;

    void sendToMasterServer(const std::string& masterIp, int masterPort);
};

#endif // SERVER_EMULATOR_H