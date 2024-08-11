#pragma once

#include <assert.h>

#include "udp/client/DirectoryConnector.hpp"

class NFControllerLogicDir {
private:
	// Conector para enviar y recibir mensajes del directorio
	std::unique_ptr<DirectoryConnector> directoryConnector;

    /**
	 * Método para consultar al directorio el nick de un peer servidor y obtener
	 * como respuesta la dirección de socket IP:puerto asociada a dicho servidor
	 * 
	 * @param nickname el nick del servidor por cuya IP:puerto se pregunta
	 * @return La dirección de socket del servidor identificado por dich nick, o
	 *         null si no se encuentra ningún usuario con ese nick que esté
	 *         sirviendo ficheros.
	 */
	std::pair<bool, InetSocketAddress> lookupServerAddrByUsername(std::string nickname);

public:
	/**
	 * Método para comprobar que la comunicación con el directorio es exitosa (se
	 * pueden enviar y recibir datagramas) haciendo uso de la clase
	 * DirectoryConnector
	 * 
	 * @param directoryHostname el nombre de host/IP en el que se está ejecutando el
	 *                          directorio
	 * @return true si se ha conseguido contactar con el directorio.
	 * @throws IOException
	 */
	void testCommunicationWithDirectory(std::string directoryHostname); // throws IOException

	/**
	 * Método para conectar con el directorio y obtener la "sessionKey" que se
	 * deberá utilizar en lo sucesivo para identificar a este cliente ante el
	 * directorio
	 * 
	 * @param directoryHostname el nombre de host/IP en el que se está ejecutando el
	 *                          directorio
	 * @return true si se ha conseguido contactar con el directorio.
	 * @throws IOException
	 */
	bool doLogin(std::string directoryHostname, std::string nickname);

	/**
	 * Método para desconectarse del directorio: cerrar sesión y dar de baja el
	 * nombre de usuario registrado
	 */
	bool doLogout();

	/**
	 * Método para obtener y mostrar la lista de nicks registrados en el directorio
	 */
	bool getAndPrintUserList();

	/**
	 * Método para obtener y mostrar la lista de ficheros que los peer servidores
	 * han publicado al directorio
	 */
	bool getAndPrintFileList();

	/**
	 * Método para registrarse en el directorio como servidor de ficheros en un
	 * puerto determinado
	 * 
	 * @param serverPort el puerto en el que está escuchando nuestro servidor de
	 *                   ficheros
	 */

	bool registerFileServer(int serverPort);

	/**
	 * Método para enviar al directorio la lista de ficheros que este peer servidor
	 * comparte con el resto (ver método filelist).
	 * 
	 */
	bool publishLocalFiles();

	/**
	 * Método para obtener la dirección de socket asociada a un servidor a partir de
	 * una cadena de caracteres que contenga: i) el nick del servidor, o ii)
	 * directamente una IP:puerto.
	 * 
	 * @param serverNicknameOrSocketAddr El nick o IP:puerto del servidor por el que
	 *                                   preguntamos
	 * @return La dirección de socket del peer identificado por dicho nick, o null
	 *         si no se encuentra ningún peer con ese nick.
	 */
	std::pair<bool, InetSocketAddress> getServerAddress(std::string serverNicknameOrSocketAddr);

	/**
	 * Método para consultar al directorio los nicknames de los servidores que
	 * tienen un determinado fichero identificado por su hash.
	 * 
	 * @param fileHashSubstring una subcadena del hash del fichero por el que se
	 *                          pregunta
	 */
	bool getAndPrintServersNicknamesSharingThisFile(std::string fileHashSubstring);

	/**
	 * Método para consultar al directorio las direcciones de socket de los
	 * servidores que tienen un determinado fichero identificado por su hash.
	 * 
	 * @param fileHashSubstring una subcadena del hash del fichero por el que se
	 *                          pregunta
	 * @return Una lista de direcciones de socket de los servidores que comparten
	 *         dicho fichero, o null si dicha subcadena del hash no identifica
	 *         ningún fichero concreto (no existe o es una subcadena ambigua)
	 * 
	 */
	std::pair<bool, std::list<InetSocketAddress>> getServerAddressesSharingThisFile(std::string downloadTargetFileHash);

	/**
	 * Método para dar de baja a nuestro servidor de ficheros en el directorio.
	 * 
	 * @return Éxito o fracaso de la operación
	 */
    bool unregisterFileServer();

	InetSocketAddress getDirectoryAddress();

};
