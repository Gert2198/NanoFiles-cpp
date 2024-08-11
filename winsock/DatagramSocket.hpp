#pragma once 

#include <iostream>

#include "DatagramPacket.hpp"
#include "SocketTimeoutException.hpp"

class DatagramSocket {
public:
    // Constructor por defecto
    DatagramSocket() : sock(INVALID_SOCKET) {
        initializeSocket();
    }

    // Constructor que acepta un puerto
    DatagramSocket(unsigned short port) : sock(INVALID_SOCKET) {
        initializeSocket();
        bind(port);
    }

    ~DatagramSocket() {
        closesocket(sock);
    }

    // Método para enlazar el socket a un puerto
    void bind(unsigned short port) {
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(port);

        if (::bind(sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
            closesocket(sock);
            throw std::exception();
        }
    }

    // Método para enviar un DatagramPacket
    void send(const DatagramPacket& packet) {
        const sockaddr_in& clientAddr = packet.getSocketAddress().getSocketAddress();
        
        int sendResult = sendto(sock, packet.getData(), packet.getLength(), 0, (SOCKADDR*)&clientAddr, sizeof(clientAddr));
        if (sendResult == SOCKET_ERROR) {
            int errorCode = WSAGetLastError();
            std::cerr << "Send failed: " << errorCode << std::endl;
            throw std::runtime_error("Send failed with error code: " + std::to_string(errorCode));
        }
    }

    // Método para establecer el tiempo máximo de espera para recibir
    void setSoTimeout(int timeout) {
        timeval tv;
        tv.tv_sec = timeout / 1000;  // segundos
        tv.tv_usec = (timeout % 1000) * 1000;  // microsegundos

        if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)) == SOCKET_ERROR) {
            std::cerr << "Failed to set socket timeout: " << WSAGetLastError() << std::endl;
        }
    }

    // Método para recibir un DatagramPacket
    // Método para recibir un DatagramPacket con manejo de timeout
    void receive(DatagramPacket& packet) {
        sockaddr_in senderAddr;
        int senderAddrSize = sizeof(senderAddr);

        int recvResult = recvfrom(sock, packet.getData(), packet.getLength(), 0, (SOCKADDR*)&senderAddr, &senderAddrSize);
        if (recvResult == SOCKET_ERROR) {
            int errorCode = WSAGetLastError();
            if (errorCode == WSAETIMEDOUT) {
                throw SocketTimeoutException("Receive operation timed out");
            } else {
                std::cerr << "Receive failed: " << errorCode << std::endl;
                throw std::exception();
            }
        } else {
            packet.setLength(recvResult);
            packet.setSocketAddress(InetSocketAddress({senderAddr.sin_addr}, ntohs(senderAddr.sin_port)));
        }
    }

private:
    SOCKET sock;

    // Método auxiliar para inicializar el socket
    void initializeSocket() {
        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (sock == INVALID_SOCKET) {
            std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
            throw std::exception();
        }
    }
};