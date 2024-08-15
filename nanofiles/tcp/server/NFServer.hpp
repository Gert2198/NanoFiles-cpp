#pragma once

#include <iostream>
#include <thread>
#include <atomic>

#include "NFServerThread.hpp"
#include "ServerSocket.hpp"  
#include "SocketTimeoutException.hpp"

class NFServer {
private:
    inline static const int PORT = 10000;
    inline static const int SERVERSOCKET_ACCEPT_TIMEOUT_MILISECS = 1000;

    std::thread serverThread;
    std::atomic<bool> stop;
    std::unique_ptr<ServerSocket> serverSocket;

public:
    NFServer();

    void run();

    void startServer();

    void stopServer();

    int getPort() const;
};