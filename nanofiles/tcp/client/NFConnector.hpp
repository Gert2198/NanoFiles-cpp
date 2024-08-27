#pragma once

#include <memory>
#include <fstream>
#include <filesystem>
#include <cstdio>

#define _WIN32_WINNT 0x0A00
#include <winsock2.h>
#include <ws2tcpip.h>
#include "util/SocketManager.hpp"
#include "InetSocketAddress.hpp"
#include "tcp/message/PeerMessage.hpp"
#include "util/FileDigest.hpp"
#include "util/FileInfo.hpp"

class NFConnector {
private:
    SOCKET serverSocket;
    InetSocketAddress serverAddr;
    std::unique_ptr<SocketManager> socketManager;
public:
    NFConnector(InetSocketAddress fserverAddr); // throws UnknownHostException, IOException

    bool downloadFile(const std::string& targetFileHashSubstr, std::fstream& file, const std::string& filePath) { // throws IOException
		boolean downloaded = false;
		/*
		 * Construir objetos PeerMessage que modelen mensajes con los valores
		 * adecuados en sus campos (atributos), según el protocolo diseñado, y enviarlos
		 * al servidor a través del "dos" del socket mediante el método
		 * writeMessageToOutputStream.
		 */
		PeerMessage send = PeerMessage(PeerMessageOps::OPCODE_DOWNLOAD_REQUEST, (char)targetFileHashSubstr.length(), targetFileHashSubstr);
		send.writeMessageToOutputStream(*socketManager);

		boolean end = false;
		
		PeerMessage receive = PeerMessage::readMessageFromInputStream(*socketManager);
		
		int operation = receive.getOpcode();
		switch (operation) {
		case PeerMessageOps::OPCODE_AMBIGUOUS_HASH:
			std::cerr << "*Error: Ambiguous hash, please be more specific" << std::endl;
			return downloaded;
		case PeerMessageOps::OPCODE_FILE_NOT_FOUND:
			std::cerr << "*Error: Hash does not exist" << std::endl;
			return downloaded;
		case PeerMessageOps::OPCODE_DOWNLOAD_RESPONSE:
			end = receive.isLast();
			// file.createNewFile(); (deberia estar creado ya)
			// fichero = new RandomAccessFile(file, "rw");
            std::filesystem::permissions(
                filePath,
                std::filesystem::perms::owner_read | std::filesystem::perms::owner_write | 
                std::filesystem::perms::group_read | std::filesystem::perms::group_write | 
                std::filesystem::perms::others_read
            );

			break;
		default:
			std::cerr << "*Error: Unknown message" << std::endl;
			return downloaded;
		}
		
		int i = 0;
		while (!end) {
            auto partialFile = receive.getFile();
			file.write(partialFile.data(), partialFile.size());
			
			receive = PeerMessage::readMessageFromInputStream(*socketManager);
			if (receive.getOpcode() != PeerMessageOps::OPCODE_DOWNLOAD_RESPONSE) {
				std::cerr << "*Error: something went wrong in the middle of the download" << std::endl;
				std::remove(filePath.c_str());
				return downloaded;
			}
			
			end = receive.isLast();
			i++;
		}
		downloaded = true;
		
		auto bytesHash = receive.getFile();
		std::string realHash(bytesHash.begin(), bytesHash.end());
		
        std::string calculatedHash = FileDigest::computeFileChecksumString(FileInfo::getAbsolutePath(filePath));
        if (calculatedHash != realHash) {
            std::cerr << "*Error: The file has been downloaded, but its hash is incorrect." << std::endl;
            downloaded = false;
            std::remove(filePath.c_str());
        }
		
		return downloaded;
	}

	void disconnect() {
		closesocket(serverSocket);
	}

	InetSocketAddress getServerAddr() {
		return serverAddr;
	}
};