#pragma once

#include "util/FileInfo.hpp"
#include "DirMessageOps.hpp"

#include <list>

class DirMessage {
private:
    inline static const char DELIMITER = ':'; // Define el delimitador
	inline static const char END_LINE = '\n'; // Define el carácter de fin de línea
	inline static const char LIST_DELIMITER = '&'; // Define el delimitador para cuando se mandan listas
	inline static const char INSIDE_DELIMITER = ';';

    /**
	 * Nombre del campo que define el tipo de mensaje (primera línea)
	 */
    inline static const std::string FIELDNAME_OPERATION = "operation";
    /*
	 * Nombres de todos los campos que pueden aparecer en los mensajes de este protocolo (formato campo:valor)
	 */
	inline static const std::string FIELDNAME_NICKNAME = "nickname";
	inline static const std::string FIELDNAME_SESSIONKEY = "sessionkey";
	inline static const std::string FIELDNAME_USERLIST = "userlist";
	inline static const std::string FIELDNAME_FILES = "files";
	inline static const std::string FIELDNAME_USERSPERFILE = "usersperfile";
	inline static const std::string FIELDNAME_HASH = "hash";
	inline static const std::string FIELDNAME_IP = "ip";
	inline static const std::string FIELDNAME_PORT = "port";

    /**
	 * Tipo del mensaje, de entre los tipos definidos en PeerMessageOps.
	 */
	std::string operation = DirMessageOps::OPERATION_INVALID;
	/*
	 * Atributos correspondientes a cada uno de los campos de los diferentes mensajes de este protocolo.
	 */
	std::string nickname;
	std::string sessionKey;
	std::string userlist;
	std::string files;
	std::string usersPerFile;
	std::string hash;
	std::string ip;
    std::string port;
    std::string fileSize;

    void setFilesFromString(std::string files);

    void setUsersPerFileFromString(std::string usersPerFile);

public:
    inline static const int PACKET_MAX_SIZE = 65507; // 65535 - 8 (UDP header) - 20 (IP header)

    DirMessage(std::string op);

	std::string getOperation();

	std::string getNickname();
	void setNickname(std::string nick);

	std::string getSessionKey();
	void setSessionKey(std::string sessionKey);
	
	std::map<std::string, bool> getUserlist();
	void setUserlist(std::map<std::string, bool> listaUsuarios);
	void setUserlistFromString(std::string userlist);
	
	std::pair<bool, std::vector<FileInfo>> getFiles();
	bool setFiles(std::vector<FileInfo> files);

	std::map<std::string, std::list<std::string>> getUsersPerFile();
	void setUsersPerFile(std::map<std::string, std::list<std::string>> usersPerFile);

	std::string getHash();
	void setHash(std::string hash);
	
	std::string getIp();
    void setIp(std::string ip);

    std::string getPort();
    void setPort(std::string port);
    
	std::string getFileSize();
	void setFileSize(std::string fileSize);

    /**
	 * Método que convierte un mensaje codificado como una cadena de caracteres, a
	 * un objeto de la clase PeerMessage, en el cual los atributos correspondientes
	 * han sido establecidos con el valor de los campos del mensaje.
	 * 
	 * @param message El mensaje recibido por el socket, como cadena de caracteres
	 * @return Un objeto PeerMessage que modela el mensaje recibido (tipo, valores,
	 *         etc.)
	 */
	static DirMessage fromString(std::string message);

    /**
	 * Método que devuelve una cadena de caracteres con la codificación del mensaje
	 * según el formato campo:valor, a partir del tipo y los valores almacenados en
	 * los atributos.
	 * 
	 * @return La cadena de caracteres con el mensaje a enviar por el socket.
	 */
	std::string toString();
};