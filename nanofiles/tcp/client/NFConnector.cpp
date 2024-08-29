#include "NFConnector.hpp"

#include <iostream>

NFConnector::NFConnector(InetSocketAddress fserverAddr) : serverSocket(fserverAddr) {
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) 
        throw std::runtime_error("*Error al crear el socket: " + WSAGetLastError());

    sockaddr_in clientAddr;
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_addr.s_addr = INADDR_ANY;  // Aceptar conexiones en cualquier dirección
    clientAddr.sin_port = htons(0);  // Permitir que el sistema operativo asigne cualquier puerto disponible

    if (bind(clientSocket, (sockaddr*)&clientAddr, sizeof(clientAddr)) == SOCKET_ERROR) {
        int error = WSAGetLastError();
        closesocket(clientSocket);
        throw std::runtime_error("Bind failed with error: " + error);
    }

    sockaddr_in addr = fserverAddr.getSocketAddress();
    if (connect(clientSocket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        int error = WSAGetLastError();
        closesocket(clientSocket);
        throw std::runtime_error("Error trying to connect to server socket: " + error);
    }
    std::cout << "Connecting to " << fserverAddr.toString() << std::endl;
    
    serverSocketManager = std::make_unique<SocketManager>(serverSocket.getSocket());
}

bool NFConnector::downloadFile(const std::string& targetFileHashSubstr, std::fstream& file, const std::string& filePath) { // throws IOException
    boolean downloaded = false;
    /*
    * Construir objetos PeerMessage que modelen mensajes con los valores
    * adecuados en sus campos (atributos), según el protocolo diseñado, y enviarlos
    * al servidor a través del "dos" del socket mediante el método
    * writeMessageToOutputStream.
    */
    PeerMessage send = PeerMessage(PeerMessageOps::OPCODE_DOWNLOAD_REQUEST, (char)targetFileHashSubstr.length(), targetFileHashSubstr);
    send.writeMessageToOutputStream(*serverSocketManager);

    boolean end = false;
    
    PeerMessage receive = PeerMessage::readMessageFromInputStream(*serverSocketManager);
    
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
        
        receive = PeerMessage::readMessageFromInputStream(*serverSocketManager);
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

void NFConnector::disconnect() {
    serverSocket.close();
}

InetSocketAddress NFConnector::getServerAddr() {
    return {serverSocket.getLocalAddress(), serverSocket.getLocalPort()};
}