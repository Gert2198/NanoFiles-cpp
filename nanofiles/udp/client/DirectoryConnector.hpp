#pragma once

#include <map>
#include <list>
#include <string>
#include <vector>
#include <memory>

#include "udp/message/DirMessage.hpp"
#include "InetSocketAddress.hpp"
#include "DatagramSocket.hpp"

class DirectoryConnector {
private:
	/**
	 * Puerto en el que atienden los servidores de directorio
	 */
	inline static const int DIRECTORY_PORT = 6868;
	/**
	 * Tiempo máximo en milisegundos que se esperará a recibir una respuesta por el
	 * socket antes de que se deba lanzar una excepción SocketTimeoutException para
	 * recuperar el control
	 */
	inline static const int TIMEOUT = 1000;
	/**
	 * Número de intentos máximos para obtener del directorio una respuesta a una
	 * solicitud enviada. Cada vez que expira el timeout sin recibir respuesta se
	 * cuenta como un intento.
	 */
	inline static const int MAX_NUMBER_OF_ATTEMPTS = 5;

    /**
	 * Socket UDP usado para la comunicación con el directorio
	 */
	std::unique_ptr<DatagramSocket> socket;
	/**
	 * Dirección de socket del directorio (IP:puertoUDP)
	 */
	std::unique_ptr<InetSocketAddress> directoryAddress;

	int sessionKey = INVALID_SESSION_KEY;
	bool successfulResponseStatus;
	std::string errorDescription;

    /**
	 * Método para enviar y recibir datagramas al/del directorio
	 * 
	 * @param requestData los datos a enviar al directorio (mensaje de solicitud)
	 * @return los datos recibidos del directorio (mensaje de respuesta)
	 */
    std::pair<bool, std::vector<char>> sendAndReceiveDatagrams(std::vector<char> requestData);

public:
	/**
	 * Valor inválido de la clave de sesión, antes de ser obtenida del directorio al loguearse
	 */
	inline static const int INVALID_SESSION_KEY = -1;

	DirectoryConnector(std::string address); // throws IOException

	/**
	 * Método para probar la comunicación con el directorio mediante el envío y
	 * recepción de mensajes sin formatear ("en crudo")
	 * 
	 * @return verdadero si se ha enviado un datagrama y recibido una respuesta
	 */
	bool testSendAndReceive();

	InetSocketAddress getDirectoryAddress();

	int getSessionKey();

	/**
	 * Método para "iniciar sesión" en el directorio, comprobar que está operativo y
	 * obtener la clave de sesión asociada a este usuario.
	 * 
	 * @param nickname El nickname del usuario a registrar
	 * @return La clave de sesión asignada al usuario que acaba de loguearse, o -1
	 *         en caso de error
	 */
	bool logIntoDirectory(std::string nickname);

	/**
	 * Método para obtener la lista de "nicknames" registrados en el directorio.
	 * Opcionalmente, la respuesta puede indicar para cada nickname si dicho peer
	 * está sirviendo ficheros en este instante.
	 * 
	 * @return La lista de nombres de usuario registrados, o null si el directorio
	 *         no pudo satisfacer nuestra solicitud
	 */
	std::pair<bool, std::map<std::string, bool>> getUserList();

	/**
	 * Método para "cerrar sesión" en el directorio
	 * 
	 * @return Verdadero si el directorio eliminó a este usuario exitosamente
	 */
	bool logoutFromDirectory();

	/**
	 * Método para dar de alta como servidor de ficheros en el puerto indicado a
	 * este peer.
	 * 
	 * @param serverPort El puerto TCP en el que este peer sirve ficheros a otros
	 * @return Verdadero si el directorio acepta que este peer se convierta en
	 *         servidor.
	 */
	bool registerServerPort(int serverPort);

	/**
	 * Método para obtener del directorio la dirección de socket (IP:puerto)
	 * asociada a un determinado nickname.
	 * 
	 * @param nick El nickname del servidor de ficheros por el que se pregunta
	 * @return La dirección de socket del servidor en caso de que haya algún
	 *         servidor dado de alta en el directorio con ese nick, o null en caso
	 *         contrario.
	 */
	std::pair<bool, InetSocketAddress> lookupServerAddrByUsername(std::string nick);

	/**
	 * Método para publicar ficheros que este peer servidor de ficheros están
	 * compartiendo.
	 * 
	 * @param files La lista de ficheros que este peer está sirviendo.
	 * @return Verdadero si el directorio tiene registrado a este peer como servidor
	 *         y acepta la lista de ficheros, falso en caso contrario.
	 */
	bool publishLocalFiles(std::vector<FileInfo> files);

	/**
	 * Método para obtener la lista de ficheros que los peers servidores han
	 * publicado al directorio. Para cada fichero se debe obtener un objeto FileInfo
	 * con nombre, tamaño y hash. Opcionalmente, puede incluirse para cada fichero,
	 * su lista de peers servidores que lo están compartiendo.
	 * 
	 * @return Los ficheros publicados al directorio, o null si el directorio no
	 *         pudo satisfacer nuestra solicitud
	 */
	std::pair<bool, std::map<FileInfo, std::list<std::string>>> getFileList();

	/**
	 * Método para obtener la lista de nicknames de los peers servidores que tienen
	 * un fichero identificado por su hash. Opcionalmente, puede aceptar también
	 * buscar por una subcadena del hash, en vez de por el hash completo.
	 * 
	 * @return La lista de nicknames de los servidores que han publicado al
	 *         directorio el fichero indicado. Si no hay ningún servidor, devuelve
	 *         una lista vacía.
	 */
	std::pair<bool, std::list<std::string>> getServerNicknamesSharingThisFile(std::string fileHash);

	bool unregisterServer();
};