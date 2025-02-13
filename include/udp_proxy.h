#ifndef UDP_PROXY_H
#define UDP_PROXY_H

#include <string>
#include <unordered_map>
#include <chrono>

class UdpProxy {
public:
    UdpProxy(int startPort, int endPort, const std::string& remoteIp, int remotePort);
    void startProxy();

private:
    int startPort;
    int endPort;
    std::string remoteServerIp;
    int remoteServerPort;
};

#endif // UDP_PROXY_H