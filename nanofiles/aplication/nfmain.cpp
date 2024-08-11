#include "NanoFiles.hpp"
#include "logic/NFController.hpp"

bool initWSAData(WSAData& wsaData) {
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return false;
    } 

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        std::cerr << "Version 2.2 of Winsock is not available." << std::endl;
        WSACleanup();
        return false;
    } 

    return true;
}

int main(int argc, char *argv[]) {
    // Comprobamos los argumentos
    if (argc > 2) {
        std::cout << "Usage: java -jar NanoFiles.jar [<local_shared_directory>]"<< std::endl;
        exit(1);
    }
    else if (argc == 2) {
        // Establecemos el directorio compartido especificado 
        NanoFiles::sharedDirname = argv[1];
    }

    NanoFiles::db = FileDatabase(NanoFiles::sharedDirname);

    WSADATA wsaData;
    if (!initWSAData(wsaData)) std::cerr << "WSA not initialized" << std::endl; 

    // Creamos el controlador que aceptará y procesará los comandos
    NFController controller;
    // Entramos en el bucle para pedirle al controlador que procese comandos del
    // shell hasta que el usuario quiera salir de la aplicación.
    do {
        controller.readGeneralCommandFromShell();
        controller.processCommand();
    } while (controller.shouldQuit() == false);
    std::cout << "Bye." << std::endl;
}