#include "NFDirectoryServer.hpp"

#include <iostream>
#include <set>

NFDirectoryServer::NFDirectoryServer(double corruptionProbability)
    : messageDiscardProbability(corruptionProbability), 
    socket(NFDirectoryServer::DIRECTORY_PORT)
{} // throws SocketException

DirMessage NFDirectoryServer::buildResponseFromRequest(DirMessage msg, InetSocketAddress clientAddr) {
		
    std::string operation = msg.getOperation();
    std::unique_ptr<DirMessage> response = nullptr;

    if (operation == DirMessageOps::OPERATION_LOGIN){
        std::string username = msg.getNickname();

        if (nicks.find(username) == nicks.end()) {
            int intKey;
            do {
                intKey = dis(gen);
            } while (sessionKeys.find(intKey) != sessionKeys.end());
            std::string newSessionKey = std::to_string(intKey);
            nicks[username] = intKey;
            sessionKeys[intKey] = username;
            
            response = std::make_unique<DirMessage>(DirMessageOps::OPERATION_LOGIN_OK);
            response->setSessionKey(newSessionKey);
            
            std::cout << "New login: " << username << ", SessionKey: " << newSessionKey << std::endl;
        } else {
            response = std::make_unique<DirMessage>(DirMessageOps::OPERATION_LOGIN_FAIL);
            std::cout << "Login already taken: " << username << std::endl;
        }
    } else if (operation == DirMessageOps::OPERATION_LOGOUT) {
        std::string sessionKey = msg.getSessionKey();
        int sessionKeyInt = std::stoi(sessionKey);
        
        if (sessionKeys.find(sessionKeyInt) != sessionKeys.end()) {
            std::string username = sessionKeys[sessionKeyInt];
            nicks.erase(username);
            sessionKeys.erase(sessionKeyInt);
            std::cout << "Remove user..." << username << " (sessionKey = " << sessionKey << ")." << std::endl;
            response = std::make_unique<DirMessage>(DirMessageOps::OPERATION_LOGOUT_OK);
        } else {
            response = std::make_unique<DirMessage>(DirMessageOps::OPERATION_NOT_LOGGED);
            std::cerr << "*Error: your sessionKey doesn't exist" << std::endl;
        }
    } else if (operation == DirMessageOps::OPERATION_USERLIST) {
        std::string sessionKey = msg.getSessionKey();
        int sessionKeyInt = std::stoi(sessionKey);
        
        if (sessionKeys.find(sessionKeyInt) != sessionKeys.end()) {
            response = std::make_unique<DirMessage>(DirMessageOps::OPERATION_USERLIST_OK);
            std::map<std::string, bool> map;
            for (auto u : nicks) {
                if (servers.find(u.first) != servers.end()) map[u.first] = true;
                else map[u.first] = false;
            }
            response->setUserlist(map);
        } else {
            response = std::make_unique<DirMessage>(DirMessageOps::OPERATION_NOT_LOGGED);
            std::cerr << "*Error: your sessionKey doesn't exist" << std::endl;
        }
    } else if (operation == DirMessageOps::OPERATION_FILELIST) {
        std::string sessionKey = msg.getSessionKey();
        int sessionKeyInt = std::stoi(sessionKey);
        
        if (sessionKeys.find(sessionKeyInt) != sessionKeys.end()) {
            response = std::make_unique<DirMessage>(DirMessageOps::OPERATION_FILELIST_OK);

            std::set<FileInfo> infoFiles;
            for (auto keyValue : usersPerFile) 
                infoFiles.insert(keyValue.first);
            
            std::vector<FileInfo> myFiles(infoFiles.size()); //fileName - fileSize - fileHash
            int i = 0;
            for (FileInfo file : infoFiles)
                myFiles[i++] = file;
            response->setFiles(myFiles);
            
            std::map<std::string, std::list<std::string>> serversPerFile;
            for (FileInfo file : infoFiles) 
                serversPerFile[file.fileHash] = usersPerFile[file];
            response->setUsersPerFile(serversPerFile);
        } else {
            response = std::make_unique<DirMessage>(DirMessageOps::OPERATION_NOT_LOGGED);
            std::cerr << "*Error: your sessionKey doesn't exist" << std::endl;
        }
    } else if (operation == DirMessageOps::OPERATION_SEARCH) {
        std::string sessionKey = msg.getSessionKey();
        int sessionKeyInt = std::stoi(sessionKey);
        
        if (sessionKeys.find(sessionKeyInt) != sessionKeys.end()) {
            std::set<FileInfo> infoFiles;
            for (auto keyValue : usersPerFile) 
                infoFiles.insert(keyValue.first);

            std::vector<FileInfo> infoFilesArray(infoFiles.size());
            int i = 0;
            for (FileInfo file : infoFiles) infoFilesArray[i++] = file;

            std::vector<FileInfo> files = FileInfo::lookupHashSubstring(infoFilesArray, msg.getHash());
            
            if (files.size() != 1) response = std::make_unique<DirMessage>(DirMessageOps::OPERATION_SEARCH_FAIL);
            else {
                response = std::make_unique<DirMessage>(DirMessageOps::OPERATION_SEARCH_OK);
            
                std::map<std::string, std::list<std::string>> serversPerFile;
                for (FileInfo file : files) 
                    serversPerFile[file.fileHash] = usersPerFile[file];
                response->setUsersPerFile(serversPerFile);
            }
        } else {
            response = std::make_unique<DirMessage>(DirMessageOps::OPERATION_NOT_LOGGED);
            std::cerr << "*Error: your sessionKey doesn't exist" << std::endl;
        }
    } else if (operation == DirMessageOps::OPERATION_PUBLISH) {
        std::string sessionKey = msg.getSessionKey();
        int sessionKeyInt = std::stoi(sessionKey);
        
        if (sessionKeys.find(sessionKeyInt) != sessionKeys.end()) {
            std::string nick = sessionKeys[sessionKeyInt];
            if (servers.find(nick) == servers.end()) {
                response = std::make_unique<DirMessage>(DirMessageOps::OPERATION_NOT_SERVER);
                std::cerr << "*Error: Client was not a server" << std::endl;
            } else {
                std::pair<bool, std::vector<FileInfo>> files = msg.getFiles();
                for (FileInfo file : files.second) {
                    if (std::find(filesPerUser[nick].begin(), filesPerUser[nick].end(), file) != filesPerUser[nick].end())
                        filesPerUser[nick].push_back(file);
                    if (usersPerFile.find(file) == usersPerFile.end()) 
                        usersPerFile[file] = std::list<std::string>();
                    if (std::find(usersPerFile[file].begin(), usersPerFile[file].end(), nick) != usersPerFile[file].end())
                        usersPerFile[file].push_back(nick);
                }
                response = std::make_unique<DirMessage>(DirMessageOps::OPERATION_PUBLISH_OK);
            }
            
        } else {
            response = std::make_unique<DirMessage>(DirMessageOps::OPERATION_NOT_LOGGED);
            std::cerr << "*Error: your sessionKey doesn't exist" << std::endl;
        }
    } else if (operation == DirMessageOps::OPERATION_GET_ADDRESS) {
        std::string sessionKey = msg.getSessionKey();
        int sessionKeyInt = std::stoi(sessionKey);
        
        if (sessionKeys.find(sessionKeyInt) != sessionKeys.end()) {
            std::string nickname = msg.getNickname();
            auto it = servers.find(nickname);
            if (it != servers.end()) {
                response = std::make_unique<DirMessage>(DirMessageOps::OPERATION_GET_ADDRESS_OK);
                response->setIp(it->second.getAddress().toString());
                response->setPort("" + it->second.getPort());
            } else 
                response = std::make_unique<DirMessage>(DirMessageOps::OPERATION_GET_ADDRESS_FAIL);
        } else {
            response = std::make_unique<DirMessage>(DirMessageOps::OPERATION_NOT_LOGGED);
            
            std::cerr << "*Error: your sessionKey doesn't exist" << std::endl;
        }
    } else if (operation == DirMessageOps::OPERATION_BGSERVE) {
        std::string sessionKey = msg.getSessionKey();
        int sessionKeyInt = std::stoi(sessionKey);
        
        if (sessionKeys.find(sessionKeyInt) != sessionKeys.end()) {
            response = std::make_unique<DirMessage>(DirMessageOps::OPERATION_BGSERVE_OK);
            std::string nickname = sessionKeys[sessionKeyInt];
            if (servers.find(nickname) == servers.end()) { //si no es ya servidor
                InetSocketAddress serverAddress(clientAddr.getAddress(), std::stoi(msg.getPort()));
                std::cout << "Meto server " << nickname << " con address " << serverAddress.toString() << std::endl;
                servers.insert({nickname, serverAddress});
                filesPerUser[nickname] = std::list<FileInfo>();
            }
        } else {
            response = std::make_unique<DirMessage>(DirMessageOps::OPERATION_NOT_LOGGED);
            std::cerr << "*Error: your sessionKey doesn't exist" << std::endl;
        }
    } else if (operation == DirMessageOps::OPERATION_STOPSERVER) {
        std::string sessionKey = msg.getSessionKey();
        int sessionKeyInt = std::stoi(sessionKey);
        
        if (sessionKeys.find(sessionKeyInt) != sessionKeys.end()) {
            std::string nick = sessionKeys[sessionKeyInt];
            if (servers.find(nick) == servers.end()) {
                response = std::make_unique<DirMessage>(DirMessageOps::OPERATION_NOT_SERVER);
                std::cerr << "*Error: Client was not a server" << std::endl;
            } else {
                response = std::make_unique<DirMessage>(DirMessageOps::OPERATION_STOPSERVER_OK);
                if (filesPerUser.find(nick) != filesPerUser.end()) {
                    std::list<FileInfo> files = filesPerUser[nick]; 
                    for (FileInfo file : files) {
                        auto it = std::find(usersPerFile[file].begin(), usersPerFile[file].end(),nick);
                        usersPerFile[file].erase(it);
                        if (usersPerFile[file].empty()) usersPerFile.erase(file);
                    }
                }
                filesPerUser.erase(nick);
                servers.erase(nick);
                std::cout << "Removed server " << nick << " with sessionKey = " << sessionKeyInt << std::endl;
            }
        } else {
            response = std::make_unique<DirMessage>(DirMessageOps::OPERATION_NOT_LOGGED);
            std::cerr << "*Error: your sessionKey doesn't exist" << std::endl;
        }
    } else {
        std::cerr << "Unexpected message operation: \"" << operation << "\"" << std::endl;
        response = std::make_unique<DirMessage>(DirMessageOps::OPERATION_INVALID);
    }
    
    return *response;
}

void NFDirectoryServer::run() { // throws IOException
    char receptionBuffer[DirMessage::PACKET_MAX_SIZE];
    std::unique_ptr<InetSocketAddress> clientAddr = nullptr;
    int dataLength = -1;

    std::cout << "Directory starting..." << std::endl;

    while (true) { // Bucle principal del servidor de directorio
        
        DatagramPacket recibido(receptionBuffer, DirMessage::PACKET_MAX_SIZE);
        socket.receive(recibido);
        dataLength = recibido.getLength();
        clientAddr = std::make_unique<InetSocketAddress>(recibido.getSocketAddress());

        std::cout << "Directory received datagram from " << clientAddr->toString() << " of size " << dataLength << " bytes" << std::endl;

        // Analizamos la solicitud y la procesamos
        if (dataLength > 0) {
            std::string messageFromClient(receptionBuffer, dataLength);
            
            // Vemos si el mensaje debe ser ignorado por la probabilidad de descarte
            // Esto devuelve un valor entre 0 y 1, ambos incluidos
            double rand = (double)std::rand() / (double)RAND_MAX;
            if (rand < messageDiscardProbability) {
                std::cerr << "Directory DISCARDED datagram from " << clientAddr->toString() << std::endl;
                continue;
            }

            std::cout << "Mensaje recibido: " << messageFromClient << std::endl;
            DirMessage mensaje = DirMessage::fromString(messageFromClient);
            
            DirMessage respuesta = buildResponseFromRequest(mensaje, *clientAddr);
            
            std::string enviar = respuesta.toString();
            std::vector<char> bytes(enviar.begin(), enviar.end());
            // bytes.push_back('\0');
            DatagramPacket enviando((char*)&bytes[0], bytes.size(), *clientAddr); 
            socket.send(enviando);
        } else {
            std::cerr << "Directory ignores EMPTY datagram from " << clientAddr->toString() << std::endl;
        }

    }
}