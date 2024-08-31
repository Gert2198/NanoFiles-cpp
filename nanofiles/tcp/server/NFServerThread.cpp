#include "NFServerThread.hpp"

void NFServerThread::run() {
    NFServerComm::serveFilesToClient(clientSocket);
}

// Constructor que toma el socket del cliente
NFServerThread::NFServerThread(SOCKET client) : clientSocket(client) {}

// Permitir movimiento
NFServerThread::NFServerThread(NFServerThread&& other) noexcept
    : clientSocket(other.clientSocket), thread(std::move(other.thread)) 
{
    other.clientSocket = INVALID_SOCKET;
}

NFServerThread& NFServerThread::operator=(NFServerThread&& other) noexcept 
{
    if (this != &other) {
        clientSocket = other.clientSocket;
        thread = std::move(other.thread);
        other.clientSocket = INVALID_SOCKET;
    }
    return *this;
}

void NFServerThread::start() {
    thread = std::thread(&NFServerThread::run, this);
    thread.detach(); 
}