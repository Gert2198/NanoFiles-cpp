#include "NFConnector.hpp"

#include <iostream>

NFConnector::NFConnector(InetSocketAddress fserverAddr) : serverAddr(fserverAddr) {
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) 
        throw std::runtime_error("Error creating server socket: " + std::to_string(WSAGetLastError()));

    sockaddr_in addr = serverAddr.getSocketAddress();

    if (::bind(serverSocket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        closesocket(serverSocket);
        throw std::runtime_error("Bind failed with error: " + std::to_string(WSAGetLastError()));
    }
    std::cout << "Connected to..." << serverAddr.getAddress().toString() << ":" << serverAddr.getPort() << std::endl;
    
    socketManager = std::make_unique<SocketManager>(serverSocket);
}