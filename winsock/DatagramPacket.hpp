#pragma once

#include "InetSocketAddress.hpp"

class DatagramPacket {
public:
    DatagramPacket(char* buf, int length, const InetSocketAddress& address)
        : buffer(buf), length(length), address(address) {}

    DatagramPacket(char* buf, int length)
        : buffer(buf), length(length), address(InetSocketAddress({"0.0.0.0"}, 0)) {}

    char* getData() const { return buffer; }
    int getLength() const { return length; }

    InetSocketAddress getSocketAddress() const { return address; }
    void setSocketAddress(const InetSocketAddress& addr) { address = addr; }

    void setLength(int len) { length = len; }

private:
    char* buffer;
    int length;
    InetSocketAddress address;
};