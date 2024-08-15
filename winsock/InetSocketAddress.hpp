#pragma once

#include "InetAddress.hpp"

class InetSocketAddress {
public:
    // Constructor que acepta una InetAddress y un puerto
    InetSocketAddress(const InetAddress& inetAddress, unsigned short port);
    InetSocketAddress(unsigned short port);

    // Obtener la dirección encapsulada como InetAddress
    InetAddress getAddress() const;

    // Obtener el puerto
    unsigned short getPort() const;

    // Obtener la estructura sockaddr_in
    const sockaddr_in& getSocketAddress() const;

    std::string toString() const;
private:
    sockaddr_in addr;
};