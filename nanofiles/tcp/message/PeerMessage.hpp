#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <assert.h>

#include "PeerMessageOps.hpp"
#include "util/SocketManager.hpp"

class PeerMessage {
private:
	unsigned char opcode;
	
	unsigned char hashLenght = 0;
	std::string fileHash;
	
	bool last = false;
	int length = 0;
	std::vector<char> file;

public:
	PeerMessage();
	PeerMessage(unsigned char op);
	PeerMessage(unsigned char op, unsigned char longitud, std::string hash);
	PeerMessage(unsigned char op, bool ultimo, int longitud, std::vector<char> fichero);

	unsigned char getOpcode();

	unsigned char getHashLenght();
	void setHashLenght(unsigned char hashLenght);

	std::string getFileHash();
	void setFileHash(std::string fileHash);
	
	bool isLast();
	void setLast(bool last);
	
	int getLength();
	void setLength(int length);

	std::vector<char> getFile();
	void setFile(std::vector<char> file);

	/**
	 * Método de clase para parsear los campos de un mensaje y construir el objeto
	 * DirMessage que contiene los datos del mensaje recibido
	 * 
	 * @param data El array de bytes recibido
	 * @return Un objeto de esta clase cuyos atributos contienen los datos del
	 *         mensaje recibido.
	 * @throws IOException
	 */
	static PeerMessage readMessageFromInputStream(SocketManager dis);

	void writeMessageToOutputStream(SocketManager dos);
};
