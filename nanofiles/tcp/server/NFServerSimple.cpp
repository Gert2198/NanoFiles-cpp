#include "NFServerSimple.hpp"

NFServerSimple::NFServerSimple() { // throws IOException
    InetSocketAddress serverAddress(PORT);

    serverSocket = std::make_unique<ServerSocket>(serverAddress); 
    serverSocket->setReuseAddress(true); // para volver a poder usar el puerto en cuanto se termine la conexion
}

/**
 * Método para ejecutar el servidor de ficheros en primer plano. Sólo es capaz
 * de atender una conexión de un cliente. Una vez se lanza, ya no es posible
 * interactuar con la aplicación a menos que se implemente la funcionalidad de
 * detectar el comando STOP_SERVER_COMMAND (opcional)
 * 
 */
void NFServerSimple::run() {
    if (serverSocket == nullptr) {
        std::cerr << "*Error: Failed to run the file server, server socket not bound to any address" << std::endl;
    }
    
    boolean stopServer = false;
    while (!stopServer) {
        try {
            SOCKET clientSocket = serverSocket->accept();
            if (clientSocket != INVALID_SOCKET) NFServerComm::serveFilesToClient(clientSocket, serverSocket->getSocket());
        } catch (const SocketTimeoutException& e1) {
            continue;
        } catch (const std::exception& e) {
            std::cerr << "*Error: Problem accepting a connection" << std::endl;
        }
    }
    
    std::cout << "NFServerSimple stopped. Returning to the nanoFiles shell..." << std::endl;
}