#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <stdexcept>
#include <string>

#include "InetSocketAddress.hpp"

class ServerSocket {
private:
    SOCKET serverSocket;
    bool bound;
public:
    ServerSocket() : serverSocket(INVALID_SOCKET), bound(false) {}

    ServerSocket(const InetSocketAddress& address) : ServerSocket() {
        bind(address);
    }

    ~ServerSocket() {
        close();
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
        SOCKET clientSocket = ::accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            int error = WSAGetLastError();
            if (error == WSAEWOULDBLOCK) {
                return INVALID_SOCKET; // Timeout or no pending connections
            } else {
                throw std::runtime_error("Accept failed with error: " + std::to_string(error));
            }
        }
        return clientSocket;
    }

    void setSoTimeout(int timeoutMillis) {
        DWORD timeout = static_cast<DWORD>(timeoutMillis);
        if (setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) == SOCKET_ERROR) 
            throw std::runtime_error("Failed to set socket timeout: " + std::to_string(WSAGetLastError()));
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
    }

    SOCKET getSocket() { return serverSocket; }
};