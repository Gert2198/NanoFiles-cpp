#pragma once

#define _WIN32_WINNT 0x0A00
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <string>

class SocketManager {
private:
    inline static const int MAX_SIZE = 32*1024;

    SOCKET socket;
public:
    SocketManager(SOCKET socket);
    

    char readByte();
    bool readBool();
    int readInt();
    void readFully(char* buf, int size);
    void readFully(std::vector<char>& buf, int size = MAX_SIZE);

    void writeByte(char value);
    void writeBool(bool value);
    void writeInt(int value);
    void write(const char* buf, int size = MAX_SIZE);
    void write(const std::vector<char>& data, int size = MAX_SIZE); 
};