#include "NFCommands.hpp"

unsigned char NFCommands::stringToCommand(std::string comStr) {
    //Busca entre los comandos si es válido y devuelve su código
    for (int i = 0; i < _valid_user_commands_str.size(); i++) {
        if (StringUtils::iequals(_valid_user_commands_str[i],comStr)) {
            return _valid_user_commands[i];
        }
    }
    //Si no se corresponde con ninguna cadena entonces devuelve el código de comando no válido
    return COM_INVALID;
}

std::pair<bool, std::string> NFCommands::commandToString(unsigned char command) {
    for (int i = 0; i < _valid_user_commands.size(); i++) {
        if (_valid_user_commands[i] == command) {
            return {true, _valid_user_commands_str[i]};
        }
    }
    return {false, {}};
}

/**
 * Imprime la lista de comandos y la ayuda de cada uno
 */
void NFCommands::printCommandsHelp() {
    std::cout << "List of commands:" << std::endl;
    for (int i = 0; i < _valid_user_commands_str.size(); i++) 
        std::cout << std::setw(15) << std::left << _valid_user_commands_str[i] << " -- " << _valid_user_commands_help[i] << std::endl;
}