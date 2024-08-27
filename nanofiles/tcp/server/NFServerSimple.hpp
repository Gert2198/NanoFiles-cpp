#pragma once

#include "ServerSocket.hpp"
#include "NFServerComm.hpp"

class NFServerSimple {
private:
    inline static const int SERVERSOCKET_ACCEPT_TIMEOUT_MILISECS = 1000;
	inline static const std::string STOP_SERVER_COMMAND = "fgstop";
	inline static const int PORT = 10000;
	std::unique_ptr<ServerSocket> serverSocket = nullptr;
public:
	NFServerSimple();

	/**
	 * Método para ejecutar el servidor de ficheros en primer plano. Sólo es capaz
	 * de atender una conexión de un cliente. Una vez se lanza, ya no es posible
	 * interactuar con la aplicación a menos que se implemente la funcionalidad de
	 * detectar el comando STOP_SERVER_COMMAND (opcional)
	 */
	void run();
};