#include "SocketManager.hpp"

SocketManager::SocketManager(SOCKET socket) {
    this->socket = socket;
}

char SocketManager::readByte() {
    char buffer[1];
    recv(socket, buffer, 1, 0);
    return buffer[0];
}
bool SocketManager::readBool() {
    return readByte() != 0; 
}
int SocketManager::readInt() {
    char buffer[4];
    recv(socket, buffer, 4, 0);
    return *(int*)buffer;
}
void SocketManager::readFully(char* buf, int size) {
    std::vector<char> vectorBuffer(size);
    readFully(vectorBuffer, size);
    std::copy(vectorBuffer.begin(), vectorBuffer.end(), buf);
}
void SocketManager::readFully(std::vector<char>& buf, int size) {
    if (size <= MAX_SIZE) {
        buf.resize(size);
        recv(socket, &buf[0], size, 0);
    }
    else {
        buf.resize(MAX_SIZE);
        recv(socket, &buf[0], MAX_SIZE, 0);
        std::vector<char> otherBuf;
        readFully(otherBuf, size - MAX_SIZE);
        buf.insert(buf.end(), otherBuf.begin(), otherBuf.end());
    }
}


void SocketManager::writeByte(char value) {
    send(socket, &value, 1, 0);   
}
void SocketManager::writeBool(bool value) {
    char val = value ? 1 : 0;
    writeByte(val);   
}
void SocketManager::writeInt(int value) {
    send(socket, (char*)&value, 4, 0);   
}
void SocketManager::write(const char* buf, int size) {
    send(socket, buf, size, 0);
}
void SocketManager::write(const std::vector<char>& buf, int size) {
    if (size <= MAX_SIZE) send(socket, &buf[0], size, 0);
    else {
        send(socket, &buf[0], MAX_SIZE, 0);
        std::vector<char> otherBuf(buf.begin() + MAX_SIZE, buf.end());
        write(otherBuf, otherBuf.size());
    } 
}