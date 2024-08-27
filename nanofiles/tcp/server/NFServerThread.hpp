#pragma once

#include <thread>
#include <utility>  // Para std::move
#include "ServerSocket.hpp"
#include "NFServerComm.hpp"

class NFServerThread {
private:
    SOCKET clientSocket;
    std::thread thread;

    void run();
public:
    // Constructor que toma el socket del cliente
    NFServerThread(SOCKET clientSocket);

    // Deshabilitar el copiado y asignación para evitar problemas con threads
    NFServerThread(const NFServerThread&) = delete;
    NFServerThread& operator=(const NFServerThread&) = delete;

    // Permitir movimiento
    NFServerThread(NFServerThread&& other) noexcept;

    NFServerThread& operator=(NFServerThread&& other) noexcept;

    void start();
};

