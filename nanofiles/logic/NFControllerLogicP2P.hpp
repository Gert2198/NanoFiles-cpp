#pragma once

#include <memory>
#include <iostream>
#include <list>

#include "InetSocketAddress.hpp"
#include "tcp/server/NFServer.hpp"

class NFControllerLogicP2P {
private:
	friend class NFController;
	std::unique_ptr<NFServer> bgFileServer;

public:
	NFControllerLogicP2P();

	/**
     * Método para arrancar un servidor de ficheros en primer plano.
     * 
     */
	void foregroundServeFiles();

	/**
	 * Método para ejecutar un servidor de ficheros en segundo plano. Debe arrancar
	 * el servidor en un nuevo hilo creado a tal efecto.
	 * 
	 * @return Verdadero si se ha arrancado en un nuevo hilo con el servidor de
	 *         ficheros, y está a la escucha en un puerto, falso en caso contrario.
	 * 
	 */
	bool backgroundServeFiles();

	/**
	 * Método para descargar un fichero del peer servidor de ficheros
	 * 
	 * @param fserverAddr    La dirección del servidor al que se conectará
	 * @param targetFileHash El hash del fichero a descargar
	 * @param localFileName  El nombre con el que se guardará el fichero descargado
	 */
	bool downloadFileFromSingleServer(InetSocketAddress fserverAddr, std::string targetFileHash, std::string localFileName);

	/**
	 * Método para descargar un fichero del peer servidor de ficheros
	 * 
	 * @param serverAddressList La lista de direcciones de los servidores a los que
	 *                          se conectará
	 * @param targetFileHash    Hash completo del fichero a descargar
	 * @param localFileName     Nombre con el que se guardará el fichero descargado
	 */
	bool downloadFileFromMultipleServers(std::list<InetSocketAddress> serverAddressList, std::string targetFileHash, std::string localFileName);

	/**
	 * Método para obtener el puerto de escucha de nuestro servidor de ficheros en
	 * segundo plano
	 * 
	 * @return El puerto en el que escucha el servidor, o 0 en caso de error.
	 */
	int getServerPort();

	/**
	 * Método para detener nuestro servidor de ficheros en segundo plano
	 * 
	 */
	void stopBackgroundFileServer();

};
