#include "NFServer.hpp"

NFServer::NFServer() : stop(false) {
    InetSocketAddress address(PORT);

    serverSocket = std::make_unique<ServerSocket>();

    serverSocket->bind(address);
    serverSocket->setSoTimeout(SERVERSOCKET_ACCEPT_TIMEOUT_MILISECS);
    serverSocket->listen();
}

void NFServer::run() {
    if (!serverSocket->isBound()) {
        std::cerr << "*Error: Failed to run the file server, server socket not bound" << std::endl;
        return;
    }

    SOCKET socket;
    while (!stop.load()) {
        try {
            socket = serverSocket->accept();
            std::cout << "\nNew client connected" << std::endl;
        } catch (const SocketTimeoutException& e1) {
            continue;
        } catch (const std::exception& e2) {
            if (!serverSocket) std::cerr << "*Error: server socket is null" << std::endl;
            std::cerr << "*Error: There was a problem with the local file server running on " << serverSocket->getLocalAddress().toString() << ":" << serverSocket->getLocalPort() << std::endl;
        }

        if (socket != INVALID_SOCKET) {
            NFServerThread connectionThread(socket);
            connectionThread.start();
        }
    }

    if (socket != INVALID_SOCKET) {
        try {
            closesocket(socket);
        } catch (const std::exception e) {
            std::cerr << "*Error: failed closing Bgserve's client socket" << std::endl;
        }
    }

    if (serverSocket) {
        try {
            serverSocket->close();
            serverSocket = nullptr; // ??
        } catch (const std::exception e) {
            std::cerr << "*Error: failed closing Bgserve's server socket" << std::endl;
        }
    }
}

void NFServer::startServer() {
    serverThread = std::thread(&NFServer::run, this);
}

void NFServer::stopServer() {
    stop.store(true);
    if (serverThread.joinable()) serverThread.join();
    else std::cerr << "*Error: thread not joinable" << std::endl;
}

int NFServer::getPort() const {
    return serverSocket->getLocalPort();
}