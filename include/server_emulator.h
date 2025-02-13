#ifndef SERVER_EMULATOR_H
#define SERVER_EMULATOR_H

#include <string>

class ServerEmulator {
public:
    ServerEmulator(const std::string& ip, int port);
    void sendMasterServerInfo();
    void listenForConnections();

private:
    std::string ip;
    int port;

    void sendToMasterServer(const std::string& masterIp, int masterPort);
};

#endif // SERVER_EMULATOR_H