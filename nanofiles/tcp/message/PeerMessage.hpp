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
	
	bool isLast = false;
	int length = 0;
	std::vector<char> file;

public:
	PeerMessage() {
		opcode = PeerMessageOps::OPCODE_INVALID_CODE;
	}

	PeerMessage(unsigned char op) {
		opcode = op;
	}
	
	
	PeerMessage(unsigned char op, unsigned char longitud, std::string hash) {
		opcode = op;
		hashLenght = longitud;
		fileHash = hash;
	}
	
	PeerMessage(unsigned char op, bool ultimo, int longitud, std::vector<char> fichero) {
		opcode = op;
		this->isLast = ultimo;
		length = longitud;
		this->file = fichero;
	}

	unsigned char getOpcode() {
		return opcode;
	}

	unsigned char getHashLenght() {
		return hashLenght;
	}

	void setHashLenght(unsigned char hashLenght) {
		assert(opcode == PeerMessageOps::OPCODE_DOWNLOAD_REQUEST);
		this->hashLenght = hashLenght;
	}

	std::string getFileHash() {
		return fileHash;
	}

	void setFileHash(std::string fileHash) {
		assert(opcode == PeerMessageOps::OPCODE_DOWNLOAD_REQUEST);
		this->fileHash = fileHash;
	}
	
	
	bool isLast() {
		return isLast;
	}

	void setLast(bool isLast) {
		assert(opcode == PeerMessageOps::OPCODE_DOWNLOAD_RESPONSE);
		this->isLast = isLast;
	}
	
	int getLength() {
		return length;
	}

	void setLength(int length) {
		assert(opcode == PeerMessageOps::OPCODE_DOWNLOAD_RESPONSE);
		this->length = length;
	}

	std::vector<char> getFile() {
		return file;
	}

	void setFile(std::vector<char> file) {
		assert(opcode == PeerMessageOps::OPCODE_DOWNLOAD_RESPONSE);
		this->file = file;
	}

	/**
	 * Método de clase para parsear los campos de un mensaje y construir el objeto
	 * DirMessage que contiene los datos del mensaje recibido
	 * 
	 * @param data El array de bytes recibido
	 * @return Un objeto de esta clase cuyos atributos contienen los datos del
	 *         mensaje recibido.
	 * @throws IOException
	 */
	static PeerMessage readMessageFromInputStream(SocketManager dis) { // throws IOException
		/*
		 * En función del tipo de mensaje, leer del socket a través del "dis" el
		 * resto de campos para ir extrayendo con los valores y establecer los atributos
		 * del un objeto PeerMessage que contendrá toda la información del mensaje, y que
		 * será devuelto como resultado. NOTA: Usar dis.readFully para leer un array de
		 * bytes, dis.readInt para leer un entero, etc.
		 */
		
		PeerMessage message;
		unsigned char opcode;
		dis.writeByte(opcode); 
		switch (opcode) {
		case PeerMessageOps::OPCODE_DOWNLOAD_REQUEST: {
			unsigned char hashLenght;
			dis.writeByte(hashLenght); 
			
			// Rehacer con vector ?? 
			char * data;
			dis.readFully(data, hashLenght);
			std::string hash(data);
			
			message = PeerMessage(opcode, hashLenght, hash);
			break;
		}
		case PeerMessageOps::OPCODE_DOWNLOAD_RESPONSE: {
			bool esfinal = dis.readBool();
			int longitud = dis.readInt();
			char * ficheroOhash;
			dis.readFully(ficheroOhash, longitud);

			std::vector<char> data(ficheroOhash, ficheroOhash + longitud);
			
			message = PeerMessage(opcode, esfinal, longitud, data);
			break;
		}
		case PeerMessageOps::OPCODE_FILE_NOT_FOUND:
		case PeerMessageOps::OPCODE_AMBIGUOUS_HASH:
			message = PeerMessage(opcode);
			break;
			
		default:
			std::cerr << "PeerMessage.readMessageFromInputStream doesn't know how to parse this message opcode: "
					<< opcode << " = " << PeerMessageOps::opcodeToOperation(opcode).second << std::endl;
			exit(-1);
		}
		return message;
	}

	void writeMessageToOutputStream(SocketManager dos) { // throws IOException
		/*
		 * Escribir los bytes en los que se codifica el mensaje en el socket a
		 * través del "dos", teniendo en cuenta opcode del mensaje del que se trata y
		 * los campos relevantes en cada caso. NOTA: Usar dos.write para leer un array
		 * de bytes, dos.writeInt para escribir un entero, etc.
		 */

		dos << opcode;
		switch (opcode) {
		case PeerMessageOps::OPCODE_DOWNLOAD_REQUEST:
			assert(hashLenght > 0 && fileHash.length() == hashLenght);

			dos << hashLenght;
			// unsigned char hashValue[] = fileHash.getBytes();
			dos << fileHash;
			break;
			
		case PeerMessageOps::OPCODE_DOWNLOAD_RESPONSE:
			assert(length > 0 && length <= 32768 && file.size() == length);
			
			dos << isLast << length;
			for (auto c : file) dos << c;
			break;
			
		case PeerMessageOps::OPCODE_FILE_NOT_FOUND:
		case PeerMessageOps::OPCODE_AMBIGUOUS_HASH:
			break;

		default:
			std::cerr << "PeerMessage.writeMessageToOutputStream found unexpected message opcode " << opcode << "("
					<< PeerMessageOps::opcodeToOperation(opcode).second << ")" << std::endl;
		}
	}
};
