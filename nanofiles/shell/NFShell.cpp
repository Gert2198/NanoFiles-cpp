#include "NFShell.hpp"

NFShell::NFShell() {
    std::cout << "NanoFiles shell" << std::endl;
    std::cout << "For help, type 'help'" << std::endl;
}

unsigned char NFShell::getCommand() {
    return command;
}

std::vector<std::string> NFShell::getCommandArguments() {
    return commandArgs;
}

void NFShell::readGeneralCommand() {
    bool validArgs;
    do {
        commandArgs = readGeneralCommandFromStdIn();
        // si el comando tiene parámetros hay que validarlos
        validArgs = validateCommandArguments(commandArgs);
    } while (!validArgs);
}

std::vector<std::string> NFShell::readGeneralCommandFromStdIn() {
    std::vector<std::string> vargs;
    while (true) {
        std::cout << "(nanoFiles@" << NanoFiles::sharedDirname << ") ";
        // obtenemos la línea tecleada por el usuario
        std::string input;
        std::getline(std::cin, input);
        StringUtils::trim(input);
        std::vector<std::string> tokens = StringUtils::split(input, ' ');
        // si no hay ni comando entonces volvemos a empezar
        if (tokens.size() == 0) 
            continue;

        // traducimos la cadena del usuario en el código de comando correspondiente
        command = NFCommands::stringToCommand(tokens[0]);
        if (verboseShell) {
            if (command != NFCommands::COM_SLEEP) 
                std::cout << input << std::endl;
            else 
                std::cout << std::endl;
        }
        skipValidateArgs = false;
        // Dependiendo del comando...
        switch (command) {
        case NFCommands::COM_INVALID: 
            // El comando no es válido
            std::cout << "Invalid command" << std::endl;
            continue;
        case NFCommands::COM_HELP:
            // Mostramos la ayuda
            NFCommands::printCommandsHelp();
            continue;
        case NFCommands::COM_SLEEP:
            // Requiere un parámetro
            if (tokens.size() > 1) {
                int seconds = std::stoi(tokens[1]);
                std::this_thread::sleep_for(std::chrono::milliseconds(seconds * 1000));
            }
            continue;
        case NFCommands::COM_QUIT:
        case NFCommands::COM_USERLIST:
        case NFCommands::COM_LOGOUT:
        case NFCommands::COM_FILELIST:
        case NFCommands::COM_PUBLISH:
        case NFCommands::COM_STOP_SERVER:
        case NFCommands::COM_MYFILES:
        case NFCommands::COM_FGSERVE:
        case NFCommands::COM_BGSERVE:
            // Estos comandos son válidos sin parámetros
            break;
        case NFCommands::COM_DOWNLOADFROM:
        case NFCommands::COM_SEARCH:
        case NFCommands::COM_DOWNLOAD:
        case NFCommands::COM_LOGIN:
            // Estos requieren un parámetro
            for (int i = 1; i < tokens.size(); i++) {
                vargs.push_back(tokens[i]);
            }
            break;
        default:
            skipValidateArgs = true;
            std::cout << "Invalid command" << std::endl;
        }
        break;
    }
    return vargs;
}

bool NFShell::validateCommandArguments(std::vector<std::string> args) {
    if (skipValidateArgs)
        return false;
    switch (command) {
    case NFCommands::COM_LOGIN:
        if (args.size() != 2) {
            std::cout << "Correct use: " << NFCommands::commandToString(command).second 
                << " <directory_server> <nickname>" << std::endl;
            return false;
        }
        break;
    case NFCommands::COM_DOWNLOADFROM:
        if (args.size() != 3) {
            std::cout << "Correct use:" << NFCommands::commandToString(command).second 
                << " <nickame/IP:port> <file_hash> <local_filename>" << std::endl;
            return false;
        }
        break;
    case NFCommands::COM_SEARCH:
        if (args.size() != 1) {
            std::cout << "Correct use: " << NFCommands::commandToString(command).second 
                << " <file_hash>" << std::endl;
            return false;
        }
        break;
    case NFCommands::COM_DOWNLOAD:
        if (args.size() != 2) {
            std::cout << "Correct use:" << NFCommands::commandToString(command).second 
                << " <file_hash> <local_filename>" << std::endl;
            return false;
        }
        break;
    default: break;
    }
    // El resto no requieren parámetro
    return true;
}

void NFShell::enableVerboseShell() {
    verboseShell = true;
}