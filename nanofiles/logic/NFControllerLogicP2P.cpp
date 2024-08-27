#include "NFControllerLogicP2P.hpp"

#include "tcp/client/NFConnector.hpp"
#include "tcp/server/NFServerSimple.hpp"

NFControllerLogicP2P::NFControllerLogicP2P() : bgFileServer(nullptr) {}

void NFControllerLogicP2P::foregroundServeFiles() {
    std::unique_ptr<NFServerSimple> serverSimple = nullptr;
    try {
        serverSimple = std::make_unique<NFServerSimple>();
    } catch (const std::exception& e) {
        std::cerr << "*Error: Unable to start the server" << std::endl;
        return;
    }
    serverSimple->run();
}

bool NFControllerLogicP2P::backgroundServeFiles() {

    if (bgFileServer) {
        std::cerr << "*Error: Server running" << std::endl;
        return false;
    }
    bool result = false;
    try {
        bgFileServer = std::make_unique<NFServer>();
    } catch (const std::exception& e) {
        bgFileServer = nullptr;
        std::cerr << "*Error: Unable to start the server" << std::endl;
        return false;
    }
    bgFileServer->startServer();
    int listeningPort = bgFileServer->getPort();	
    if (listeningPort > 0) {
        result = true;
        std::cout << "NFServer server running on " + listeningPort << std::endl;
    } else std::cout << "*Error: Unable to start the server" << std::endl;
    return result;

}

bool NFControllerLogicP2P::downloadFileFromSingleServer(InetSocketAddress fserverAddr, std::string targetFileHash, std::string localFileName) {
    bool result = false;

    std::unique_ptr<NFConnector> nfConnector = nullptr;
    try {
        nfConnector = std::make_unique<NFConnector>(fserverAddr);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        nfConnector = nullptr;
    } 
    if (!nfConnector) {
        std::cerr << "*Error: Unable to connect to the server" << std::endl;
        return false;
    }
    
    std::string filePath = NanoFiles::DEFAULT_SHARED_DIRNAME + "/" + localFileName;
    if (FileInfo::fileExists(filePath)) {
        std::cerr << "*Error: The destination file already exists" << std::endl;
        return false;
    }

    std::fstream f(filePath, std::ios::app | std::ios::in | std::ios::out | std::ios::binary);
    try {
        result = nfConnector->downloadFile(targetFileHash, f, filePath);
    } catch (const std::exception& e) {
        std::cout << "*Error: exception occurred while downloading the file" << std::endl;
    }
    if (result) {
        std::cout << "The file has been downloaded successfully" << std::endl;
    } else {
        std::cerr << "*Error: It was not possible to download the file" << std::endl;
    }
    nfConnector->disconnect();

    return result;
}

bool NFControllerLogicP2P::downloadFileFromMultipleServers(std::list<InetSocketAddress> serverAddressList, std::string targetFileHash, std::string localFileName) {
    boolean downloaded = false;

    if (serverAddressList.empty()) {
        std::cerr << "*Error: Cannot start download - No list of server addresses provided" << std::endl;
        return false;
    }
    
    //NO LO VAMOS A IMPLEMENTAR
    
    /*
    * Crear un objeto NFConnector para establecer la conexión con cada
    * servidor de ficheros, y usarlo para descargar un trozo (chunk) del fichero
    * mediante su método "downloadFileChunk". Se debe comprobar previamente si ya
    * existe un fichero con el mismo nombre en esta máquina, en cuyo caso se
    * informa y no se realiza la descarga. Si todo va bien, imprimir mensaje
    * informando de que se ha completado la descarga.
    */
    /*
    * Las excepciones que puedan lanzarse deben ser capturadas y tratadas en
    * este método. Si se produce una excepción de entrada/salida (error del que no
    * es posible recuperarse), se debe informar sin abortar el programa
    */

    return downloaded;
}

int NFControllerLogicP2P::getServerPort() {
    if(!bgFileServer) return 0;
    return bgFileServer->getPort();
}

void NFControllerLogicP2P::stopBackgroundFileServer() {
    if (bgFileServer) bgFileServer->stopServer();
    bgFileServer = nullptr;
}