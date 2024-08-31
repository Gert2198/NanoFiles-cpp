#include "NFServerComm.hpp"

#include "util/SocketManager.hpp"

void NFServerComm::serveFilesToClient(SOCKET clientSocket) {
    try {
        SocketManager clientSocketManager(clientSocket);
        
        PeerMessage received = PeerMessage::readMessageFromInputStream(clientSocketManager);
        PeerMessage messageToSend;
        
        int opcode = received.getOpcode();
        FileInfo ficheroADescargar;
        
        if (opcode == PeerMessageOps::OPCODE_DOWNLOAD_REQUEST) {
            std::vector<FileInfo> files = NanoFiles::db.getFiles();
            std::vector<FileInfo> posibles = FileInfo::lookupHashSubstring(files, received.getFileHash());
            if (posibles.size() == 0) {
                messageToSend = PeerMessage(PeerMessageOps::OPCODE_FILE_NOT_FOUND);
                messageToSend.writeMessageToOutputStream(clientSocketManager); 
                closesocket(clientSocket);
                clientSocket = INVALID_SOCKET;
                return;
            } else if (posibles.size() > 1) {
                messageToSend = PeerMessage(PeerMessageOps::OPCODE_AMBIGUOUS_HASH);
                messageToSend.writeMessageToOutputStream(clientSocketManager); 
                closesocket(clientSocket);
                clientSocket = INVALID_SOCKET;
                return;
            } else ficheroADescargar = posibles[0];
        } else {
            std::cerr << "*Error: Unexpected operation from client. Closing connection" << std::endl;
            closesocket(clientSocket);
            clientSocket = INVALID_SOCKET;
            return;
        }
        
        std::string filepath = NanoFiles::db.lookupFilePath(ficheroADescargar.fileHash); 
        std::vector<char> bytes = FileInfo::readFile(filepath);
        long fichero_len = bytes.size();
        long numVeces = fichero_len / (32*1024); 
        int bytesRestantes = (int) (fichero_len % (32*1024)); 
        std::vector<char>::iterator it = bytes.begin();
        for (int i = 0; i < numVeces; i++) {
            std::vector<char> minipart(it, it + 32*1024 - 1);
            messageToSend = PeerMessage(PeerMessageOps::OPCODE_DOWNLOAD_RESPONSE, false, 32*1024, minipart);
            messageToSend.writeMessageToOutputStream(clientSocketManager); 
            it += 32*1024;
        }
        // ultimo mensaje con los bytes restantes por leer
        std::vector<char> minipart(it, bytes.end());
        messageToSend = PeerMessage(PeerMessageOps::OPCODE_DOWNLOAD_RESPONSE, false, bytesRestantes, minipart);
        messageToSend.writeMessageToOutputStream(clientSocketManager); 
        
        // ultimo mensaje indicando que es el final
        std::string filehash = ficheroADescargar.fileHash;
        std::vector<char> vectorHash(filehash.begin(), filehash.end());
        messageToSend = PeerMessage(PeerMessageOps::OPCODE_DOWNLOAD_RESPONSE, true, vectorHash.size(), vectorHash);
        messageToSend.writeMessageToOutputStream(clientSocketManager); 
        
        closesocket(clientSocket); 
        clientSocket = INVALID_SOCKET;
        
    } catch (const std::exception& e) { 
        std::cerr << "*Error: Something went wrong in P2P connection, " << e.what() << std::endl;
    }
}