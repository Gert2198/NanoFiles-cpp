#pragma once

#define _WIN32_WINNT 0x0A00
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <stdexcept>
#include <string>

#include "InetSocketAddress.hpp"
#include "SocketTimeoutException.hpp"

class ServerSocket {
private:
    SOCKET serverSocket;
    bool bound;
    int timeoutMs;
public:
    ServerSocket() : serverSocket(INVALID_SOCKET), bound(false) {}

    ServerSocket(const InetSocketAddress& address) : ServerSocket() {
        bind(address);
    }

    ~ServerSocket() {
        close();
    }

    void setReuseAddress(bool reuse) {
        if (serverSocket == INVALID_SOCKET) {
            throw std::runtime_error("Socket not initialized");
        }

        int optionValue = reuse ? 1 : 0;
        if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&optionValue, sizeof(optionValue)) == SOCKET_ERROR) {
            throw std::runtime_error("Failed to set SO_REUSEADDR option: " + std::to_string(WSAGetLastError()));
        }
    }

    void bind(const InetSocketAddress& address) {
        if (bound) 
            throw std::runtime_error("Socket is already bound");

        serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (serverSocket == INVALID_SOCKET) 
            throw std::runtime_error("Error creating server socket: " + std::to_string(WSAGetLastError()));

        sockaddr_in addr = address.getSocketAddress();

        if (::bind(serverSocket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
            close();
            throw std::runtime_error("Bind failed with error: " + std::to_string(WSAGetLastError()));
        }

        bound = true;
    }

    void listen(int backlog = SOMAXCONN) {
        if (!bound) 
            throw std::runtime_error("Cannot listen on a socket that is not bound");

        if (::listen(serverSocket, backlog) == SOCKET_ERROR) {
            throw std::runtime_error("Listen failed with error: " + std::to_string(WSAGetLastError()));
        }
    }

    SOCKET accept() {
        // Usar select() para manejar el tiempo de espera
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(serverSocket, &readfds);

        timeval timeout;
        timeout.tv_sec = timeoutMs / 1000;
        timeout.tv_usec = (timeoutMs % 1000) * 1000;

        int result = select(0, &readfds, NULL, NULL, (timeoutMs > 0) ? &timeout : NULL);

        if (result == 0) {
            throw SocketTimeoutException("Timeout exceeded");
        } else if (result == SOCKET_ERROR) {
            throw std::runtime_error("Select failed: " + std::to_string(WSAGetLastError()));
        }

        // Si select retorna un valor positivo, el socket está listo para aceptar una conexión
        SOCKET clientSocket = ::accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            throw std::runtime_error("Socket accept failed: " + std::to_string(WSAGetLastError()));
        }

        return clientSocket;
    }

    void setSoTimeout(int timeoutMillis) {
        timeoutMs = timeoutMillis;
    }

    void close() {
        if (serverSocket != INVALID_SOCKET) {
            closesocket(serverSocket);
            serverSocket = INVALID_SOCKET;
            bound = false;
        }
    }

    bool isBound() const {
        return bound;
    }

    int getLocalPort() const {
        if (!bound) return 0; 

        sockaddr_in addr = {};
        int addrLen = sizeof(addr);
        if (getsockname(serverSocket, (sockaddr*)&addr, &addrLen) == 0) {
            return ntohs(addr.sin_port);
        }
        return 0;
    }

    InetAddress getLocalAddress() const {
        if (!bound) return InetAddress("0.0.0.0");
        
        sockaddr_in addr = {};
        int addrLen = sizeof(addr);
        if (getsockname(serverSocket, (sockaddr*)&addr, &addrLen) == 0) 
            return InetAddress(addr.sin_addr);
        else return InetAddress("0.0.0.0");
    }

    SOCKET getSocket() { return serverSocket; }
};