#include "DirectoryConnector.hpp"

#include <set>
#include <assert.h>

DirectoryConnector::DirectoryConnector(std::string address) { // throws IOException
    InetAddress host(address);
    directoryAddress = std::make_unique<InetSocketAddress>(host, DirectoryConnector::DIRECTORY_PORT);
    
    socket = std::make_unique<DatagramSocket>();
}

/**
 * Método para enviar y recibir datagramas al/del directorio
 * 
 * @param requestData los datos a enviar al directorio (mensaje de solicitud)
 * @return los datos recibidos del directorio (mensaje de respuesta)
 */
std::pair<bool, std::vector<char>> DirectoryConnector::sendAndReceiveDatagrams(std::vector<char> requestData) {
    std::vector<char> responseBuffer(DirMessage::PACKET_MAX_SIZE);
    std::vector<char> response;
    if (!directoryAddress) {
        std::cerr << "DirectoryConnector.sendAndReceiveDatagrams: UDP server destination address is null!" << std::endl;
        std::cerr << "DirectoryConnector.sendAndReceiveDatagrams: make sure constructor initializes field \"directoryAddress\"" << std::endl;
        exit(-1);

    }
    if (!socket) {
        std::cerr << "DirectoryConnector.sendAndReceiveDatagrams: UDP socket is null!" << std::endl;
        std::cerr << "DirectoryConnector.sendAndReceiveDatagrams: make sure constructor initializes field \"socket\"" << std::endl;
        exit(-1);
    }
    
    //Envio mensaje
    DatagramPacket envio((char*)&requestData[0], requestData.size(), *directoryAddress); // TODO: puede que esté mal coger la direccion de un vector
    try {
        socket->send(envio);
    } catch (std::exception e) { 
        std::cerr << "Error de salida: no se ha podido enviar el mensaje" << std::endl;
        return {false, {}};
    }
    
    //Recibo mensaje
    DatagramPacket recibido((char*)&responseBuffer[0], responseBuffer.size());
    int intentos = 0;
    while (intentos < MAX_NUMBER_OF_ATTEMPTS) {
        try {
            socket->setSoTimeout(TIMEOUT);
            socket->receive(recibido);
            break;
        } catch (const SocketTimeoutException& e) {
            intentos++;
        } catch (const std::exception& e) {
            std::cerr << "Error de entrada: no se ha podido recibir el mensaje" << std::endl;
            return {false, {}};
        }
    }
    if (intentos == MAX_NUMBER_OF_ATTEMPTS) {
        std::cerr << "No se ha recibido ningún mensaje" << std::endl; 
        return {false, {}};
    }
    
    //Convierto mensaje
    std::string mensajeCompleto(recibido.getData(), recibido.getLength());
    response = std::vector<char>(mensajeCompleto.begin(), mensajeCompleto.end());
    
    if (mensajeCompleto.size() == responseBuffer.size()) {
        std::cerr << mensajeCompleto << std::endl;
        std::cerr << "Your response is as large as the datagram reception buffer!!" << std::endl
                  << "You must extract from the buffer only the bytes that belong to the datagram!" << std::endl;
    }

    return {true, response};
}

/**
 * Método para probar la comunicación con el directorio mediante el envío y
 * recepción de mensajes sin formatear ("en crudo")
 * 
 * @return verdadero si se ha enviado un datagrama y recibido una respuesta
 */
bool DirectoryConnector::testSendAndReceive() {
    bool success;
    
    std::string login = "login";
    std::vector<char> Blogin(login.begin(), login.end());
    auto Bloginok = sendAndReceiveDatagrams(Blogin);

    char * data = &Bloginok.second[0];
    std::string recibido(data, Bloginok.second.size());
    std::string loginok = "loginok";
    success = loginok == recibido;

    return success;
}

InetSocketAddress DirectoryConnector::getDirectoryAddress() {
    return *directoryAddress;
}

int DirectoryConnector::getSessionKey() {
    return sessionKey;
}

/**
 * Método para "iniciar sesión" en el directorio, comprobar que está operativo y
 * obtener la clave de sesión asociada a este usuario.
 * 
 * @param nickname El nickname del usuario a registrar
 * @return La clave de sesión asignada al usuario que acaba de loguearse, o -1
 *         en caso de error
 */
bool DirectoryConnector::logIntoDirectory(std::string nickname) {
    assert(sessionKey == INVALID_SESSION_KEY);
    bool success = false;
    
    DirMessage msgLogin(DirMessageOps::OPERATION_LOGIN);
    msgLogin.setNickname(nickname);
    
    std::string strToSend = msgLogin.toString();
    
    std::vector<char> dataToSend(strToSend.begin(), strToSend.end());
    
    auto dataReceived = sendAndReceiveDatagrams(dataToSend);

    if (dataReceived.first) {
        int longRecibidos = dataReceived.second.size();
        if (longRecibidos > 0) {
            std::string strRecibidos(&dataReceived.second[0], longRecibidos);
            DirMessage rcvMsg = DirMessage::fromString(strRecibidos);
            if (rcvMsg.getOperation() == DirMessageOps::OPERATION_LOGIN_OK) {
                sessionKey = std::stoi(rcvMsg.getSessionKey());
                std::cout << "Login success. SessionKey = " << sessionKey << std::endl;
                success = true;
            } else if (rcvMsg.getOperation() == DirMessageOps::OPERATION_LOGIN_FAIL) {
                std::cerr << "*Error: Username already taken" << std::endl;
            }
        }
    }
    return success;
}

/**
 * Método para obtener la lista de "nicknames" registrados en el directorio.
 * Opcionalmente, la respuesta puede indicar para cada nickname si dicho peer
 * está sirviendo ficheros en este instante.
 * 
 * @return La lista de nombres de usuario registrados, o null si el directorio
 *         no pudo satisfacer nuestra solicitud
 */
std::pair<bool, std::map<std::string, bool>> DirectoryConnector::getUserList() {
    std::pair<bool, std::map<std::string, bool>> userlist = {false, {}};
    
    DirMessage request(DirMessageOps::OPERATION_USERLIST);
    request.setSessionKey(std::to_string(sessionKey));
    
    std::string strToSend = request.toString();
    
    std::vector<char> dataToSend(strToSend.begin(), strToSend.end());
    
    auto dataReceived = sendAndReceiveDatagrams(dataToSend);

    if (dataReceived.first) {
        int longRecibidos = dataReceived.second.size();
        if (longRecibidos > 0) {
            std::string strRecibidos(&dataReceived.second[0], longRecibidos);
            DirMessage rcvMsg = DirMessage::fromString(strRecibidos);
            if (rcvMsg.getOperation() == DirMessageOps::OPERATION_USERLIST_OK) {
                userlist = {true, rcvMsg.getUserlist()};
            } else if (rcvMsg.getOperation() == DirMessageOps::OPERATION_NOT_LOGGED) {
                std::cerr << "*Error: SessionKey not registered" << std::endl;
            }
        }
    }
    return userlist;
}

/**
 * Método para "cerrar sesión" en el directorio
 * 
 * @return Verdadero si el directorio eliminó a este usuario exitosamente
 */
bool DirectoryConnector::logoutFromDirectory() {
    bool success = false;
    
    DirMessage request(DirMessageOps::OPERATION_LOGOUT);
    request.setSessionKey(std::to_string(sessionKey));
    
    std::string strToSend = request.toString();
    
    std::vector<char> dataToSend(strToSend.begin(), strToSend.end());
    
    auto dataReceived = sendAndReceiveDatagrams(dataToSend);

    if (dataReceived.first) {
        int longRecibidos = dataReceived.second.size();
        if (longRecibidos > 0) {
            std::string strRecibidos(&dataReceived.second[0], longRecibidos);
            DirMessage rcvMsg = DirMessage::fromString(strRecibidos);
            if (rcvMsg.getOperation() == DirMessageOps::OPERATION_LOGOUT_OK) {
                sessionKey = INVALID_SESSION_KEY;
                std::cout << "Logout successful." << std::endl;
                success = true;
            } else if (rcvMsg.getOperation() == DirMessageOps::OPERATION_NOT_LOGGED) {
                std::cerr << "*Error: SessionKey not registered" << std::endl;
            }
        }
    }
    return success;
}

/**
 * Método para dar de alta como servidor de ficheros en el puerto indicado a
 * este peer.
 * 
 * @param serverPort El puerto TCP en el que este peer sirve ficheros a otros
 * @return Verdadero si el directorio acepta que este peer se convierta en
 *         servidor.
 */
bool DirectoryConnector::registerServerPort(int serverPort) {
    assert (serverPort != 0);
    bool success = false;
    
    DirMessage message(DirMessageOps::OPERATION_BGSERVE);
    message.setSessionKey(std::to_string(sessionKey));
    message.setPort(std::to_string(serverPort));
    
    std::string strToSend = message.toString();
    
    std::vector<char> dataToSend(strToSend.begin(), strToSend.end());
    
    auto dataReceived = sendAndReceiveDatagrams(dataToSend);

    if (dataReceived.first) {
        int longRecibidos = dataReceived.second.size();
        if (longRecibidos > 0) {
            std::string strRecibidos(&dataReceived.second[0], longRecibidos);
            DirMessage rcvMsg = DirMessage::fromString(strRecibidos);
            if (rcvMsg.getOperation() == DirMessageOps::OPERATION_BGSERVE_OK) {
                std::cout << "Server registered into Directory" << std::endl;
                success = true;
            } else if (rcvMsg.getOperation() == DirMessageOps::OPERATION_NOT_LOGGED) {
                std::cerr << "*Error: You are not logged" << std::endl;
            }
        }
    }
    
    return success;
}

/**
 * Método para obtener del directorio la dirección de socket (IP:puerto)
 * asociada a un determinado nickname.
 * 
 * @param nick El nickname del servidor de ficheros por el que se pregunta
 * @return La dirección de socket del servidor en caso de que haya algún
 *         servidor dado de alta en el directorio con ese nick, o null en caso
 *         contrario.
 */
std::pair<bool, InetSocketAddress> DirectoryConnector::lookupServerAddrByUsername(std::string nick) {
    DirMessage message(DirMessageOps::OPERATION_GET_ADDRESS);
    message.setNickname(nick);
    message.setSessionKey(std::to_string(sessionKey));
    
    std::string strToSend = message.toString();
    
    std::vector<char> dataToSend(strToSend.begin(), strToSend.end());
    
    auto dataReceived = sendAndReceiveDatagrams(dataToSend);

    if (dataReceived.first) {
        int longRecibidos = dataReceived.second.size();
        if (longRecibidos > 0) {
            std::string strRecibidos(&dataReceived.second[0], longRecibidos);
            DirMessage rcvMsg = DirMessage::fromString(strRecibidos);
            if (rcvMsg.getOperation() == DirMessageOps::OPERATION_GET_ADDRESS_OK) {
                try {
                    InetAddress address(rcvMsg.getIp());
                    InetSocketAddress serverAddr(address, std::stoi(rcvMsg.getPort()));
                    return {true, serverAddr};
                } catch (const std::exception& e) {
                    std::cerr << "*Error: invalid IP" << std::endl;
                }
            } else if (rcvMsg.getOperation() == DirMessageOps::OPERATION_GET_ADDRESS_FAIL) {
                std::cerr << "*Error: Nickname given is not serving" << std::endl;
            } else if (rcvMsg.getOperation() == DirMessageOps::OPERATION_NOT_LOGGED) {
                std::cerr << "*Error: You are not logged" << std::endl;
            }
        }
    }
    return { false, { {"0.0.0.0"}, 0 } };
}

/**
 * Método para publicar ficheros que este peer servidor de ficheros están
 * compartiendo.
 * 
 * @param files La lista de ficheros que este peer está sirviendo.
 * @return Verdadero si el directorio tiene registrado a este peer como servidor
 *         y acepta la lista de ficheros, falso en caso contrario.
 */
bool DirectoryConnector::publishLocalFiles(std::vector<FileInfo> files) {
    bool success = false;
    
    if (files.size() <= 0) {
        std::cerr << "*Error: No files to publish" << std::endl;
        return success;
    }
    
    DirMessage request(DirMessageOps::OPERATION_PUBLISH);
    request.setSessionKey(std::to_string(sessionKey));
    request.setFiles(files);
    
    std::string strToSend = request.toString();
    
    std::vector<char> dataToSend(strToSend.begin(), strToSend.end());
    
    auto dataReceived = sendAndReceiveDatagrams(dataToSend);

    if (dataReceived.first) {
        int longRecibidos = dataReceived.second.size();
        if (longRecibidos > 0) {
            std::string strRecibidos(&dataReceived.second[0], longRecibidos);
            DirMessage rcvMsg = DirMessage::fromString(strRecibidos);
            if (rcvMsg.getOperation() == DirMessageOps::OPERATION_PUBLISH_OK) {
                std::cout << "Files published!" << std::endl;
                success = true;
            } else if (rcvMsg.getOperation() == DirMessageOps::OPERATION_NOT_SERVER) {
                std::cerr << "*Error: You were not serving" << std::endl;
            } else if (rcvMsg.getOperation() == DirMessageOps::OPERATION_NOT_LOGGED) {
                std::cerr << "*Error: SessionKey not registered" << std::endl;
            }
        }
    }
    return success;
}

/**
 * Método para obtener la lista de ficheros que los peers servidores han
 * publicado al directorio. Para cada fichero se debe obtener un objeto FileInfo
 * con nombre, tamaño y hash. Opcionalmente, puede incluirse para cada fichero,
 * su lista de peers servidores que lo están compartiendo.
 * 
 * @return Los ficheros publicados al directorio, o null si el directorio no
 *         pudo satisfacer nuestra solicitud
 */
std::pair<bool, std::map<FileInfo, std::list<std::string>>> DirectoryConnector::getFileList() {
    std::pair<bool, std::map<FileInfo, std::list<std::string>>> result{false, {}};

    DirMessage request(DirMessageOps::OPERATION_FILELIST);
    request.setSessionKey(std::to_string(sessionKey));
    
    std::string strToSend = request.toString();
    
    std::vector<char> dataToSend(strToSend.begin(), strToSend.end());
    
    auto dataReceived = sendAndReceiveDatagrams(dataToSend);

    if (dataReceived.first) {
        int longRecibidos = dataReceived.second.size();
        if (longRecibidos > 0) {
            std::string strRecibidos(&dataReceived.second[0], longRecibidos);
            DirMessage rcvMsg = DirMessage::fromString(strRecibidos);
            if (rcvMsg.getOperation() == DirMessageOps::OPERATION_FILELIST_OK) {
                std::map<FileInfo, std::list<std::string>> fileList;
                std::pair<bool, std::vector<FileInfo>> files = rcvMsg.getFiles(); 
                if (!files.first) {
                    std::cout << "No files uploaded" << std::endl;
                    return result;
                }
                std::map<std::string, std::list<std::string>> users = rcvMsg.getUsersPerFile();
                for (FileInfo f : files.second) 
                    fileList[f] = users[f.fileHash];

                result = {true, fileList};
            } else if (rcvMsg.getOperation() == DirMessageOps::OPERATION_NOT_LOGGED) {
                std::cerr << "*Error: SessionKey not registered" << std::endl;
            }
        }
    }
    return result;
}

/**
 * Método para obtener la lista de nicknames de los peers servidores que tienen
 * un fichero identificado por su hash. Opcionalmente, puede aceptar también
 * buscar por una subcadena del hash, en vez de por el hash completo.
 * 
 * @return La lista de nicknames de los servidores que han publicado al
 *         directorio el fichero indicado. Si no hay ningún servidor, devuelve
 *         una lista vacía.
 */
std::pair<bool, std::list<std::string>> DirectoryConnector::getServerNicknamesSharingThisFile(std::string fileHash) {
    std::pair<bool, std::list<std::string>> nicklist{false, {}};
    
    DirMessage message(DirMessageOps::OPERATION_SEARCH);
    message.setHash(fileHash);
    message.setSessionKey(std::to_string(sessionKey));
    
    std::string strToSend = message.toString();
    
    std::vector<char> dataToSend(strToSend.begin(), strToSend.end());
    
    auto dataReceived = sendAndReceiveDatagrams(dataToSend);

    if (dataReceived.first) {
        int longRecibidos = dataReceived.second.size();
        if (longRecibidos > 0) {
            std::string strRecibidos(&dataReceived.second[0], longRecibidos);
            DirMessage rcvMsg = DirMessage::fromString(strRecibidos);
            if (rcvMsg.getOperation() == DirMessageOps::OPERATION_SEARCH_OK) {
                std::map<std::string, std::list<std::string>> mapa = rcvMsg.getUsersPerFile(); // Solo tiene una entrada
                for (auto s : mapa) {
                    return {true, s.second};
                }
            } else if (rcvMsg.getOperation() == DirMessageOps::OPERATION_SEARCH_FAIL) {
                std::cerr << "*Error: hash is ambiguous or does not exists" << std::endl;
            } else if (rcvMsg.getOperation() == DirMessageOps::OPERATION_NOT_LOGGED) {
                std::cerr << "*Error: You are not logged" << std::endl;
            }
        }
    }
    
    return nicklist;
}

bool DirectoryConnector::unregisterServer() {
    bool result = false;
    
    DirMessage request(DirMessageOps::OPERATION_STOPSERVER);
    request.setSessionKey(std::to_string(sessionKey));
    
    std::string strToSend = request.toString();
    
    std::vector<char> dataToSend(strToSend.begin(), strToSend.end());
    
    auto dataReceived = sendAndReceiveDatagrams(dataToSend);

    if (dataReceived.first) {
        int longRecibidos = dataReceived.second.size();
        if (longRecibidos > 0) {
            std::string strRecibidos(&dataReceived.second[0], longRecibidos);
            DirMessage rcvMsg = DirMessage::fromString(strRecibidos);
            if (rcvMsg.getOperation() == DirMessageOps::OPERATION_STOPSERVER_OK) {
                std::cout << "You are not serving now" << std::endl;
                result = true;
            }
            else if (rcvMsg.getOperation() == DirMessageOps::OPERATION_NOT_SERVER) {
                std::cerr << "*Error: You were not serving" << std::endl;
            } else if (rcvMsg.getOperation() == DirMessageOps::OPERATION_NOT_LOGGED) {
                std::cerr << "*Error: SessionKey not registered" << std::endl;
            }
        }
    }
    
    return result;
}