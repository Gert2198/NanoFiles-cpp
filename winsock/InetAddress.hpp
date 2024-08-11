#pragma once

#define _WIN32_WINNT 0x0A00
#include <winsock2.h>
#include <ws2tcpip.h>

#include <string>
#include <exception>

class InetAddress {
public:
    // Constructor que acepta una dirección IP como string
    InetAddress(const std::string& IP) : ip(IP) {
        if (IP == "localhost") 
            ip = "127.0.0.1";
        
        address.S_un.S_addr = inet_addr(ip.c_str());
        
        if (address.S_un.S_addr == INADDR_NONE) {
            std::__throw_runtime_error("Invalid IP address format");
        }
    }

    // Constructor que acepta una dirección IP en formato sockaddr_in
    InetAddress(const in_addr& addr) : address(addr) {
        ip = inet_ntoa(address);
    }

    // Método que devuelve la dirección IP como string
    std::string getHostAddress() const {
        return ip;
    }

    in_addr getAddress() const {
        return address;
    }

    std::string toString() const {
        return getHostAddress();
    }

private:
    std::string ip;
    in_addr address;
};