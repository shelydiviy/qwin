#include "server_emulator.h"
#include "utils.h"

ServerEmulator::ServerEmulator(const std::string& ipParam, int portParam) // Переименовываем параметры
    : ip(ipParam), port(portParam), bound(false), ioContext(), socket(ioContext, asio::ip::udp::endpoint(asio::ip::udp::v4(), portParam)) {}

bool ServerEmulator::isBound() const {
    return bound;
}

void ServerEmulator::listenForConnections() {
    try {
        logMessage("Listening on " + ip + ":" + std::to_string(port), "server");

        ioContext.run();

        while (true) {
            sendMasterServerInfo();
            std::this_thread::sleep_for(std::chrono::seconds(60));
        }
    } catch (const std::exception& e) {
        logMessage("Error in server on port " + std::to_string(port) + ": " + std::string(e.what()), "error");
    }
}

void ServerEmulator::sendMasterServerInfo() {
    std::vector<std::pair<std::string, int>> masterServers = {
        {"hl2master.steampowered.com", 27011},
        {"208.64.200.55", 27011}
    };

    for (const auto& [masterIp, masterPort] : masterServers) {
        sendToMasterServer(masterIp, masterPort);
    }
}

void ServerEmulator::sendToMasterServer(const std::string& masterIp, int masterPort) {
    logMessage("Sending info to Master Server at " + masterIp + ":" + std::to_string(masterPort), "server");

    try {
        struct MasterPacket {
            uint8_t header = 0x66;
            uint8_t challenge = 0x00;
            uint8_t protocol = 48;
            uint8_t serverType = 'd';
            uint8_t platform = 'l';
            char gameDir[56] = "cstrike";
            char description[64] = "CS 1.6 Dedicated Mirror Server | CSDM | Sentry+Laser";
            char mapName[16] = "de_dust2";
            char gameName[16] = "Counter-Strike";
            uint8_t players = 0;
            uint8_t maxPlayers = 32;
            uint8_t bots = 0;
            uint8_t dedicated = 'd';
            uint8_t os = 'l';
            uint8_t passwordProtected = 0;
            uint8_t hasMods = 1;
        };

        MasterPacket packet;
        std::vector<uint8_t> buffer(sizeof(MasterPacket));
        memcpy(buffer.data(), &packet, sizeof(MasterPacket));

        asio::ip::udp::resolver resolver(ioContext);
        asio::ip::udp::endpoint endpoint = *resolver.resolve(masterIp, std::to_string(masterPort)).begin();

        socket.send_to(asio::buffer(buffer), endpoint);
        logMessage("Successfully sent info to Master Server", "server");
    } catch (const std::exception& e) {
        logMessage("Failed to send data to Master Server: " + std::string(e.what()), "error");
    }
}