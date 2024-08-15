#include "InetSocketAddress.hpp"

InetSocketAddress::InetSocketAddress(const InetAddress& inetAddress, unsigned short port) {
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr = inetAddress.getAddress();
}

InetSocketAddress::InetSocketAddress(unsigned short port) {
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.S_un.S_addr = INADDR_ANY;
}

// Obtener la dirección encapsulada como InetAddress
InetAddress InetSocketAddress::getAddress() const {
    return InetAddress(addr.sin_addr);
}

// Obtener el puerto
unsigned short InetSocketAddress::getPort() const {
    return ntohs(addr.sin_port);
}

// Obtener la estructura sockaddr_in
const sockaddr_in& InetSocketAddress::getSocketAddress() const {
    return addr;
}

std::string InetSocketAddress::toString() const {
    return getAddress().toString() + ":" + std::to_string(getPort());
}