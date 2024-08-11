#include "DirMessage.hpp"

#include <assert.h>
#include "util/StringUtils.hpp"

void DirMessage::setFilesFromString(std::string files) { 
    this->files = files; 
}

void DirMessage::setUsersPerFileFromString(std::string usersPerFile) { 
    this->usersPerFile = usersPerFile; 
}

DirMessage::DirMessage(std::string op) : operation(op) {}

std::string DirMessage::getOperation() {
    return operation;
}

std::string DirMessage::getNickname() {
    return nickname;
}
void DirMessage::setNickname(std::string nick) {
    assert(operation == DirMessageOps::OPERATION_LOGIN);
    this->nickname = nick;
}

std::string DirMessage::getSessionKey() {
    return sessionKey;
}
void DirMessage::setSessionKey(std::string sessionKey) {
    assert(operation == DirMessageOps::OPERATION_LOGIN_OK || operation == DirMessageOps::OPERATION_USERLIST ||
            operation == DirMessageOps::OPERATION_LOGOUT || operation == DirMessageOps::OPERATION_BGSERVE ||
            operation == DirMessageOps::OPERATION_GET_ADDRESS || operation == DirMessageOps::OPERATION_PUBLISH ||
            operation == DirMessageOps::OPERATION_FILELIST || operation == DirMessageOps::OPERATION_SEARCH ||
            operation == DirMessageOps::OPERATION_STOPSERVER);
    this->sessionKey = sessionKey;
}

std::map<std::string, bool> DirMessage::getUserlist() {
    std::map<std::string, bool> mapa;
    std::vector<std::string> lista = StringUtils::split(userlist, LIST_DELIMITER);
    for (std::string line : lista) {
        int idx = line.find(INSIDE_DELIMITER);
        std::string nickname = line.substr(0, idx);
        std::string serving = line.substr(idx + 1);
        bool isServing = serving == "1" ? true : false;
        mapa[nickname] = isServing;
    }
    return mapa;
}
void DirMessage::setUserlist(std::map<std::string, bool> listaUsuarios) {
    std::ostringstream sb;
    for (auto s : listaUsuarios) {
        sb << s.first << INSIDE_DELIMITER << s.second << LIST_DELIMITER;
    }
    userlist = sb.str();
}
void DirMessage::setUserlistFromString(std::string userlist) {
    this->userlist = userlist;
}

std::pair<bool, std::vector<FileInfo>> DirMessage::getFiles() {
    if (files.find(LIST_DELIMITER) == std::string::npos) return {false, {}};
    std::vector<std::string> lista = StringUtils::split(files, LIST_DELIMITER);
    
    std::vector<FileInfo> ficheros(lista.size());
    for (int i = 0; i < lista.size(); i++) {
        ficheros[i] = FileInfo();
        
        std::string cadena(lista[i]);
        std::vector<std::string> aux = StringUtils::split(cadena, INSIDE_DELIMITER);
        
        ficheros[i].fileName = aux[0];
        ficheros[i].fileSize = std::stoi(aux[1]);
        ficheros[i].fileHash = aux[2];
    }
    return {true, ficheros};
}
bool DirMessage::setFiles(std::vector<FileInfo> files) {
    std::ostringstream sb;
    for (int i = 0; i < files.size(); i++) {
        //fileName - fileSize - fileHash
        if(files[i].fileName.find(LIST_DELIMITER) != std::string::npos || 
            files[i].fileName.find(INSIDE_DELIMITER) != std::string::npos ||
            files[i].fileName.find(DELIMITER) != std::string::npos) {
            std::cerr << "*Error: fileName cannot contain characters: " << LIST_DELIMITER << INSIDE_DELIMITER << DELIMITER << std::endl;
            return false;
        }
        sb << files[i].fileName << INSIDE_DELIMITER << files[i].fileSize << INSIDE_DELIMITER << files[i].fileHash << LIST_DELIMITER;
    }
    this->files = sb.str();
    return true;
}

std::map<std::string, std::list<std::string>> DirMessage::getUsersPerFile() {
    std::map<std::string, std::list<std::string>> mapa;
    std::vector<std::string> lista = StringUtils::split(usersPerFile, LIST_DELIMITER);
    for (std::string line : lista) {
        int idx = line.find(INSIDE_DELIMITER);
        std::string hash = line.substr(0, idx);
        std::string users = line.substr(idx + 1);
        mapa[hash] = std::list<std::string>();
        std::vector<std::string> vec = StringUtils::split(users, INSIDE_DELIMITER);
        for (std::string s : vec) 
            mapa[hash].push_back(s);
    }
    return mapa;
}
void DirMessage::setUsersPerFile(std::map<std::string, std::list<std::string>> usersPerFile) {
    std::ostringstream sb;
    for (auto s : usersPerFile) {
        sb << s.first << INSIDE_DELIMITER;
        for (std::string ss : s.second) {
            sb << ss << INSIDE_DELIMITER;
        }
        int idx = sb.str().find_last_of(INSIDE_DELIMITER);
        std::string str = sb.str();
        str.erase(idx);
        str.push_back(LIST_DELIMITER);
        sb = std::ostringstream(str);
    }
    this->usersPerFile = sb.str();
}

std::string DirMessage::getHash() {
    return hash;
}
void DirMessage::setHash(std::string hash) {
    this->hash = hash;
}

std::string DirMessage::getIp() {
    return ip;
}
void DirMessage::setIp(std::string ip) {
    this->ip = ip;
}

std::string DirMessage::getPort() {
    return port;
}
void DirMessage::setPort(std::string port) {
    this->port = port;
}

std::string DirMessage::getFileSize() {
    return fileSize;
}
void DirMessage::setFileSize(std::string fileSize) {
    this->fileSize = fileSize;
}

DirMessage DirMessage::fromString(std::string message) {
    std::vector<std::string> lines = StringUtils::split(message, END_LINE); 
    
    std::unique_ptr<DirMessage> m;

    for (std::string line : lines) { 
        if (line == "") continue;
        int idx = line.find(DELIMITER); // Posición del delimitador
        std::string fieldName = line.substr(0, idx); // minúsculas
        std::transform(fieldName.begin(), fieldName.end(), fieldName.begin(), [](unsigned char c){ return std::tolower(c); });

        std::string value = line.substr(idx + 1);
        StringUtils::trim(value);

        if (fieldName == FIELDNAME_OPERATION) { // esto va a pasar siempre la primera vez, por lo que nickname y sessionkey van a inicializarse a lo que le hayamos puesto
            assert (!m); // si hay doble operation salta esto
            m = std::make_unique<DirMessage>(value);
        }
        else if (fieldName == FIELDNAME_NICKNAME) {
            assert(m); 
            m->setNickname(value);
        }
        else if (fieldName == FIELDNAME_SESSIONKEY) {
            assert(m);
            m->setSessionKey(value);
        }
        else if (fieldName == FIELDNAME_USERLIST) {
            assert(m); 
            m->setUserlistFromString(value);
        }
        else if (fieldName == FIELDNAME_FILES) {
            assert(m); 
            m->setFilesFromString(value);
        }
        else if (fieldName == FIELDNAME_USERSPERFILE) {
            assert(m); 
            m->setUsersPerFileFromString(value);
        }
        else if (fieldName == FIELDNAME_IP) {
            assert(m);
            m->setIp(value);
        }
        else if (fieldName == FIELDNAME_PORT) {
            assert(m);
            m->setPort(value);
        }
        else if (fieldName == FIELDNAME_HASH) {
            assert(m); 
            m->setHash(value);
        } else {
            std::cerr << "PANIC: DirMessage.fromString - message with unknown field name " << fieldName << std::endl;
            std::cerr << "Message was:\n" << message << std::endl;
            exit(-1);
        }
    }

    return *m.get();
}

std::string DirMessage::toString() {
    std::ostringstream sb;
    sb << FIELDNAME_OPERATION << DELIMITER << operation << END_LINE; // Construimos el campo
    
    if (operation == DirMessageOps::OPERATION_LOGIN) {
        sb << FIELDNAME_NICKNAME << DELIMITER << nickname << END_LINE;
    } else if (operation == DirMessageOps::OPERATION_LOGIN_OK || operation == DirMessageOps::OPERATION_LOGOUT ||
                operation == DirMessageOps::OPERATION_USERLIST || operation == DirMessageOps::OPERATION_FILELIST ||
                operation == DirMessageOps::OPERATION_STOPSERVER) {
        sb << FIELDNAME_SESSIONKEY << DELIMITER << sessionKey << END_LINE;
    }
    else if (operation == DirMessageOps::OPERATION_USERLIST_OK) {
        sb << FIELDNAME_USERLIST << DELIMITER << userlist << END_LINE;
    }
    else if (operation == DirMessageOps::OPERATION_PUBLISH) {
        sb << FIELDNAME_SESSIONKEY << DELIMITER << sessionKey << END_LINE << FIELDNAME_FILES << DELIMITER << files << END_LINE;
    }
    else if (operation == DirMessageOps::OPERATION_FILELIST_OK) {
        sb << FIELDNAME_FILES << DELIMITER << files << END_LINE << FIELDNAME_USERSPERFILE << DELIMITER << usersPerFile << END_LINE;
    }
    else if (operation == DirMessageOps::OPERATION_SEARCH) {
        sb << FIELDNAME_SESSIONKEY << DELIMITER << sessionKey << END_LINE << FIELDNAME_HASH << DELIMITER << hash << END_LINE;
    }
    else if (operation == DirMessageOps::OPERATION_SEARCH_OK) {
        sb << FIELDNAME_USERSPERFILE << DELIMITER << usersPerFile << END_LINE;
    }
    else if (operation == DirMessageOps::OPERATION_GET_ADDRESS) {
        sb << FIELDNAME_SESSIONKEY << DELIMITER << sessionKey << END_LINE << FIELDNAME_NICKNAME << DELIMITER << nickname << END_LINE;
    }
    else if (operation == DirMessageOps::OPERATION_GET_ADDRESS_OK) {
        sb << FIELDNAME_IP << DELIMITER << ip << END_LINE << FIELDNAME_PORT << DELIMITER << port << END_LINE;
    }
    else if (operation == DirMessageOps::OPERATION_BGSERVE) {
        sb << FIELDNAME_SESSIONKEY << DELIMITER << sessionKey << END_LINE << FIELDNAME_PORT << DELIMITER << port << END_LINE;
    }
    

    sb << END_LINE; // Marcamos el final del mensaje
    return sb.str();
}