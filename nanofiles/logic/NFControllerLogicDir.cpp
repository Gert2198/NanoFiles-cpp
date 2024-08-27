#include "NFControllerLogicDir.hpp"

#include "aplication/NanoFiles.hpp"
#include "util/StringUtils.hpp"

std::pair<bool, InetSocketAddress> NFControllerLogicDir::lookupServerAddrByUsername(std::string nickname) {
    if (!directoryConnector) return {false, InetSocketAddress({"0.0.0.0"}, 0)};
    auto serverAddr = directoryConnector->lookupServerAddrByUsername(nickname);

    return serverAddr;
}

void NFControllerLogicDir::testCommunicationWithDirectory(std::string directoryHostname) { // throws IOException
    assert (NanoFiles::testMode);
    std::cout << "[testMode] Testing communication with directory..." << std::endl;
    /*
    * Crea un objeto DirectoryConnector a partir del parámetro directoryHostname y
    * lo utiliza para hacer una prueba de comunicación con el directorio.
    */
    directoryConnector = std::make_unique<DirectoryConnector>(directoryHostname);
    if (directoryConnector->testSendAndReceive()) {
        std::cout << "[testMode] Test PASSED!" << std::endl;
    } else {
        std::cerr << "[testMode] Test FAILED!" << std::endl;
    }
}

bool NFControllerLogicDir::doLogin(std::string directoryHostname, std::string nickname) {
    bool result = false;
    
    bool comunicationOk = true;
    try {
        directoryConnector = std::make_unique<DirectoryConnector>(directoryHostname);
    } catch (const std::exception& e) {
        comunicationOk = false;
        std::cerr << "No se ha podido conectar con el directorio" << std::endl;
    }

    if (comunicationOk) 
        result = directoryConnector->logIntoDirectory(nickname);

    return result;
}

bool NFControllerLogicDir::doLogout() {
    bool result = directoryConnector->logoutFromDirectory();
    if (result) directoryConnector = nullptr;
    return result;
}

bool NFControllerLogicDir::getAndPrintUserList() {
    assert(directoryConnector);
    
    boolean result = false;
    auto userlist = directoryConnector->getUserList();
    if (!userlist.first) return result;
    
    result = true;
    for (auto user : userlist.second) {
        std::cout << "User: " << user.first << std::endl;
        std::cout << "\tServing: " << user.second << std::endl;
    }
    return result;
}

bool NFControllerLogicDir::getAndPrintFileList() {
    assert(directoryConnector);
    
    bool result = false;
    auto fileList = directoryConnector->getFileList();
    if (!fileList.first) return result;
    
    result = true;
    for (auto file : fileList.second) {
        std::cout << "Name: " << file.first.fileName << "\tSize: " << file.first.fileSize << " bytes\tHash: " << file.first.fileHash << std::endl;
        std::cout << "\tServers: ";
        for (std::string s : file.second) std::cout << s << ", ";
        std::cout << std::endl;
    }
    return result;
}

bool NFControllerLogicDir::registerFileServer(int serverPort) {
    boolean result = false;

    result = directoryConnector->registerServerPort(serverPort);

    return result;
}

bool NFControllerLogicDir::publishLocalFiles() {
    boolean result = directoryConnector->publishLocalFiles(NanoFiles::db.getFiles());

    return result;
}

std::pair<bool, InetSocketAddress> NFControllerLogicDir::getServerAddress(std::string serverNicknameOrSocketAddr) {
    std::pair<bool, InetSocketAddress> fserverAddr = {false, InetSocketAddress({"0.0.0.0"}, 0)};
    /*
    * Averiguar si el nickname es en realidad una cadena "IP:puerto", en cuyo
    * caso no es necesario comunicarse con el directorio (simplemente se devuelve
    * un InetSocketAddress); en otro caso, utilizar el método
    * lookupServerAddrByUsername de esta clase para comunicarse con el directorio y
    * obtener la IP:puerto del servidor con dicho nickname. Devolver null si la
    * operación fracasa.
    */
    auto it = serverNicknameOrSocketAddr.find(":");
    if (it != std::string::npos) { // Then it has to be a socket address (IP:port)
        std::string ip = serverNicknameOrSocketAddr.substr(0, it);
        StringUtils::trim(ip);
        std::string port = serverNicknameOrSocketAddr.substr(it + 1);
        StringUtils::trim(port);
        fserverAddr = {true, InetSocketAddress(ip, std::stoi(port))};
    } else {
        /*
        * Si es un nickname, preguntar al directorio la IP:puerto asociada a
        * dicho peer servidor.
        */
        fserverAddr = lookupServerAddrByUsername(serverNicknameOrSocketAddr);
    }
    return fserverAddr;
}

bool NFControllerLogicDir::getAndPrintServersNicknamesSharingThisFile(std::string fileHashSubstring) {
    bool result = false;
    
    auto nicknames = directoryConnector->getServerNicknamesSharingThisFile(fileHashSubstring);
    if (!nicknames.first) return result;
    std::cout << "Servers sharing the file with starting hash: " << fileHashSubstring << std::endl;
    for (std::string s : nicknames.second) std::cout << "\t" << s << std::endl;

    return result;
}

std::pair<bool, std::list<InetSocketAddress>> NFControllerLogicDir::getServerAddressesSharingThisFile(std::string downloadTargetFileHash) {
    std::pair<bool, std::list<InetSocketAddress>> serverAddressList = {false, {}};
    
    // No implementado
    
    return serverAddressList;
}

bool NFControllerLogicDir::unregisterFileServer() {
    boolean result = false;

    result = directoryConnector->unregisterServer();

    return result;
}

InetSocketAddress NFControllerLogicDir::getDirectoryAddress() {
    return directoryConnector->getDirectoryAddress();
}