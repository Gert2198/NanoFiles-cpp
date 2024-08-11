#pragma once

#include <map>
#include <list>
#include <random>

#include "udp/message/DirMessage.hpp"

#include "InetSocketAddress.hpp"
#include "DatagramSocket.hpp"

class NFDirectoryServer {
private:
	/**
	 * Socket de comunicación UDP con el cliente UDP (DirectoryConnector)
	 */
	DatagramSocket socket;
	/**
	 * Estructura para guardar los nicks de usuarios registrados, y clave de sesión
	 */
	std::map<std::string, int> nicks;
	/**
	 * Estructura para guardar las claves de sesión y sus nicks de usuario asociados
	 */
	std::map<int, std::string> sessionKeys;
	
	std::map<std::string, InetSocketAddress> servers; // asocia nickname con IP:Puerto
	std::map<std::string, std::list<FileInfo>> filesPerUser; // para cada servidor, los ficheros que ha subido
	std::map<FileInfo, std::list<std::string>> usersPerFile; // para cada fichero, los servidores que lo han subido

	/**
	 * Generador de claves de sesión aleatorias (sessionKeys)
	 */
	// WARNING: Random no existe en C++, asiq lo haremos con std::rand en vez de crear una variable Random
	// Crear un generador de números aleatorios
    inline static std::random_device rd;
    inline static std::mt19937 gen = std::mt19937(rd());
    
    inline static std::uniform_int_distribution<> dis = std::uniform_int_distribution<>(1, 1000);

	/**
	 * Probabilidad de descartar un mensaje recibido en el directorio (para simular
	 * enlace no confiable y testear el código de retransmisión)
	 */
	double messageDiscardProbability;

    DirMessage buildResponseFromRequest(DirMessage msg, InetSocketAddress clientAddr);

public:
    /**
	 * Número de puerto UDP en el que escucha el directorio
	 */
	inline static const int DIRECTORY_PORT = 6868;

    NFDirectoryServer(double corruptionProbability); // throws SocketException

    void run(); // throws IOException
};