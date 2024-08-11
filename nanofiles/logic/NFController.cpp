#include "NFController.hpp"

void NFController::updateCurrentState(bool success) {
    if (!success) return;

    switch(currentCommand) {
    case NFCommands::COM_LOGIN:
        currentState = LOGGED_IN;
        break;
    case NFCommands::COM_LOGOUT:
        currentState = LOGGED_OUT;
        break;
    default:
        break;
    }
}

void NFController::showMyLocalFiles() {
    std::cout << "List of files in local folder:" << std::endl;
    FileInfo::printToSysout(NanoFiles::db.getFiles());
}

void NFController::setCurrentCommand(unsigned char command) {
    currentCommand = command;
}

void NFController::setCurrentCommandArguments(std::vector<std::string> args) {
    switch(currentCommand) {
    case NFCommands::COM_LOGIN:
        directory = args[0];
        nickname = args[1];
        break;
    case NFCommands::COM_SEARCH:
        downloadTargetFileHash = args[0];
        break;
    case NFCommands::COM_DOWNLOADFROM:
        downloadTargetServer = args[0];
        downloadTargetFileHash = args[1];
        downloadLocalFileName = args[2];
        break;
    case NFCommands::COM_DOWNLOAD:
        downloadTargetFileHash = args[0];
        downloadLocalFileName = args[1];
        break;
    default:
        break;
    }
}

NFController::NFController() {
    // controllerPeer = NFControllerLogicP2P();
    // Estado inicial del autómata
    currentState = LOGGED_OUT;
}

void NFController::processCommand() {

    if (!canProcessCommandInCurrentState()) 
        return;
    /*
        * En función del comando, invocar los métodos adecuados de NFControllerLogicDir
        * y NFControllerLogicP2P, ya que son estas dos clases las que implementan
        * realmente la lógica de cada comando y procesan la información recibida
        * mediante la comunicación con el directorio u otros pares de NanoFiles
        * (imprimir por pantalla el resultado de la acción y los datos recibidos,
        * etc.).
        */
    bool commandSucceeded = false;

    switch(currentCommand) {
    case NFCommands::COM_MYFILES: 
        showMyLocalFiles(); // Muestra los ficheros en el directorio local compartido
        break;
    case NFCommands::COM_LOGIN:
        if (NanoFiles::testMode) {
            try {
                controllerDir.testCommunicationWithDirectory(directory);
                return;
            } catch (const std::exception& e) {
                std::cerr << "[testMode] An error occurred, failed to communicate with directory" << std::endl;
                exit(-1);
            }
        }
        /*
        * Pedir al controllerDir que "inicie sesión" en el directorio, para comprobar
        * que está activo y disponible, y registrar un nombre de usuario.
        */
        commandSucceeded = controllerDir.doLogin(directory, nickname);
        break;
    case NFCommands::COM_LOGOUT: 
        /*
        * Pedir al controllerDir que "cierre sesión" en el directorio para dar de baja
        * el nombre de usuario registrado (método doLogout).
        */
        // TODO
        // if (controllerPeer.bgFileServer != null) {
        // 	System.err.println("*Error: cannot logout while serving, please stop the server");
        // 	commandSucceeded = false;
        // } else {
            commandSucceeded = controllerDir.doLogout();
        // }
        break;
    case NFCommands::COM_USERLIST:
        /*
        * Pedir al controllerDir que obtenga del directorio la lista de nicknames
        * registrados, y la muestre por pantalla (método getAndPrintUserList)
        */
        commandSucceeded = controllerDir.getAndPrintUserList();
        break;
    case NFCommands::COM_FILELIST:
        /*
        * Pedir al controllerDir que obtenga del directorio la lista de ficheros que
        * hay publicados (los ficheros que otros peers están sirviendo), y la imprima
        * por pantalla (método getAndPrintFileList)
        */
        commandSucceeded = controllerDir.getAndPrintFileList();
        break;
    case NFCommands::COM_FGSERVE:
        /*
        * Pedir al controllerPeer que lance un servidor de ficheros en primer plano
        * (método foregroundServeFiles). Este método no retorna...
        */
        // controllerPeer.foregroundServeFiles();               TODO
        break;
    case NFCommands::COM_PUBLISH:
        /*
        * Pedir al controllerDir que publique en el directorio nuestra lista de
        * ficheros disponibles (NanoFiles.db) para ser descargados desde otros peers
        * (método publishLocalFiles)
        */
        commandSucceeded = controllerDir.publishLocalFiles();
        break;
    case NFCommands::COM_BGSERVE:
        /*
        * Pedir al controllerPeer que lance un servidor en segundo plano. A
        * continuación (método backgroundServeFiles). Si el servidor se ha podido
        * iniciar correctamente, pedir al controllerDir darnos de alta como servidor de
        * ficheros en el directorio, indicando el puerto en el que nuestro servidor
        * escucha conexiones de otros peers (método registerFileServer).
        */
        // bool serverRunning = controllerPeer.backgroundServeFiles();       TODO
        // if (serverRunning) {
        // 	commandSucceeded = controllerDir.registerFileServer(controllerPeer.getServerPort());
        // }
        break;
    case NFCommands::COM_STOP_SERVER:
        /*
        * Pedir al controllerPeer que pare el servidor en segundo plano (método método
        * stopBackgroundFileServer). A continuación, pedir al controllerDir que
        * solicite al directorio darnos de baja como servidor de ficheros (método
        * unregisterFileServer).
        */
        // TODO
        // controllerPeer.stopBackgroundFileServer();
        // commandSucceeded = controllerDir.unregisterFileServer();
        break;
    case NFCommands::COM_DOWNLOADFROM:
        /*
        * Pedir al controllerDir que obtenga del directorio la dirección de socket (IP
        * y puerto) del servidor cuyo nickname indica el atributo downloadTargetServer
        * (1er argumento pasado al comando en el shell). Si se ha obtenido del
        * directorio la dirección del servidor de ficheros asociada al nick
        * exitosamente, pedir al controllerPeer que descargue del servidor en dicha
        * dirección el fichero indicado en downloadTargetFileHash (2º argumento pasado
        * al comando) y lo guarde con el nombre indicado en downloadLocalFileName (3er
        * argumento)
        */
        // TODO
        // auto serverAddr = controllerDir.getServerAddress(downloadTargetServer);
        // commandSucceeded = controllerPeer.downloadFileFromSingleServer(serverAddr, downloadTargetFileHash, downloadLocalFileName);
        break;
    case NFCommands::COM_SEARCH:
        /*
        * Pedir al controllerDir que obtenga del directorio y muestre los servidores
        * que tienen disponible el fichero identificado por dicho hash (puede ser una
        * subcadena del hash o el hash completo)
        */
        commandSucceeded = controllerDir.getAndPrintServersNicknamesSharingThisFile(downloadTargetFileHash);
        break;
    case NFCommands::COM_DOWNLOAD:
        /*
        * Pedir al controllerDir que obtenga del directorio la lista de nicknames de
        * servidores que comparten el fichero indicado en downloadTargetFileHash (1er
        * argumento pasado al comando). Si existen servidores que comparten dicho
        * fichero, pedir al controllerPeer que descargue el fichero indicado de los
        * servidores obtenidos, y lo guarde con el nombre indicado en
        * downloadLocalFileName (2º argumento)
        */
        // TODO
        // auto serverAddressList = controllerDir.getServerAddressesSharingThisFile(downloadTargetFileHash);
        // commandSucceeded = controllerPeer.downloadFileFromMultipleServers(serverAddressList.second, downloadTargetFileHash, downloadLocalFileName);
        break;
    case NFCommands::COM_QUIT:
    default:
        break;
    }

    updateCurrentState(commandSucceeded);
}

bool NFController::canProcessCommandInCurrentState() {
    bool commandAllowed = true;
    switch(currentCommand) {
    case NFCommands::COM_MYFILES:
        commandAllowed = true;
        break;
    case NFCommands::COM_LOGIN:
        if (currentState != LOGGED_OUT) {
            commandAllowed = false;
            std::cerr << "* You cannot login because you are not logged out from the directory" << std::endl;
        }
        break;
    case NFCommands::COM_LOGOUT:
    case NFCommands::COM_BGSERVE: 
    case NFCommands::COM_STOP_SERVER:
    case NFCommands::COM_PUBLISH:
    case NFCommands::COM_FILELIST:
    case NFCommands::COM_SEARCH:
    case NFCommands::COM_USERLIST:
        if (currentState != LOGGED_IN) {
            commandAllowed = false;
            std::cerr << "* You have to be logged in to do that" << std::endl;
        }
        break;
    case NFCommands::COM_QUIT:
        if (currentState != LOGGED_OUT) {
            commandAllowed = false;
            std::cerr << "..." << std::endl;
        }
        break;
    default:
        break;
    }
    return commandAllowed;
}

bool NFController::shouldQuit() {
    return currentCommand == NFCommands::COM_QUIT && currentState == LOGGED_OUT;
}

void NFController::readGeneralCommandFromShell() {
    // Pedimos el comando al shell
    shell.readGeneralCommand();
    // Establecemos que el comando actual es el que ha obtenido el shell
    setCurrentCommand(shell.getCommand());
    // Analizamos los posibles parámetros asociados al comando
    setCurrentCommandArguments(shell.getCommandArguments());
}