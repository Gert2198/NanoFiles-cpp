#pragma once

#include "shell/NFShell.hpp"
#include "NFControllerLogicDir.hpp"
#include "NFControllerLogicP2P.hpp"

class NFController {
private:
    /**
	 * Diferentes estados del cliente de acuerdo con el autómata
	 */
    inline static const unsigned char LOGGED_OUT = 0;
    inline static const unsigned char LOGGED_IN = 1;

	/**
	 * Shell para leer comandos de usuario de la entrada estándar
	 */
	NFShell shell;
	/**
	 * Último comando proporcionado por el usuario
	 */
	unsigned char currentCommand;

	/**
	 * Objeto controlador encargado de la comunicación con el directorio
	 */
	NFControllerLogicDir controllerDir;
	/**
	 * Objeto controlador encargado de la comunicación con otros peers (como
	 * servidor o cliente)
	 */
	NFControllerLogicP2P controllerPeer;

	/**
	 * El estado en que se encuentra este peer (según el autómata). El estado debe
	 * actualizarse cuando se produce un evento (comando) que supone un cambio en el
	 * autómata.
	 */
	unsigned char currentState;
	/**
	 * Atributos donde se establecen los argumentos pasados a los distintos comandos
	 * del shell. Estos atributos se establecen automáticamente según la orden y se
	 * deben usar para pasar los valores de los parámetros a las funciones invocadas
	 * desde este controlador.
	 */
	std::string nickname; // Nick del usuario (register)
	std::string directory; // Nombre/IP del host donde está el directorio (login)
	std::string downloadTargetFileHash; // Hash del fichero a descargar (download)
	std::string downloadLocalFileName; // Nombre con el que se guardará el fichero descargado
	std::string downloadTargetServer; // nombre o IP:puerto del sevidor del que se descargará el fichero

    void updateCurrentState(bool success);

	void showMyLocalFiles();

    /**
	 * Establece el comando actual
	 * 
	 * @param command el comando tecleado en el shell
	 */
	void setCurrentCommand(unsigned char command);

	/**
	 * Registra en atributos internos los posibles parámetros del comando tecleado
	 * por el usuario.
	 */
	void setCurrentCommandArguments(std::vector<std::string> args);

public:
    // Constructor
	NFController();

	/**
	 * Método que procesa los comandos introducidos por un usuario. Se encarga
	 * principalmente de invocar los métodos adecuados de NFControllerLogicDir y
	 * NFControllerLogicP2P según el comando.
	 */
	void processCommand();

	/**
	 * Método que comprueba si se puede procesar un comando introducidos por un
	 * usuario, en función del estado del autómata en el que nos encontramos.
	 */
	bool canProcessCommandInCurrentState();

	/**
	 * Método que comprueba si el usuario ha introducido el comando para salir de la
	 * aplicación
	 */
	bool shouldQuit();


	/**
	 * Método para leer un comando general
	 */
	void readGeneralCommandFromShell();
};