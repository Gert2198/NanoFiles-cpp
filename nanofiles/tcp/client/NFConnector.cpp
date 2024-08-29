#include "NFConnector.hpp"

#include <iostream>

NFConnector::NFConnector(InetSocketAddress fserverAddr) : serverAddr(fserverAddr) {
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) 
        throw std::runtime_error("Error creating server socket: " + std::to_string(WSAGetLastError()));

    sockaddr_in addr = serverAddr.getSocketAddress();

    if (::connect(serverSocket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        int error = WSAGetLastError();
        closesocket(serverSocket);
        throw std::runtime_error("Bind failed with error: " + std::to_string(error));
    }
    std::cout << "Connecting to " << serverAddr.toString() << std::endl;
    
    socketManager = std::make_unique<SocketManager>(serverSocket);
}