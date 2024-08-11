#pragma once

#include <iostream>
#include <vector>
#include <iomanip>

#include "util/StringUtils.hpp"

class NFCommands {
public:
	/**
	 * Códigos para todos los comandos soportados por el shell
	 */
	inline static const unsigned char COM_INVALID = 0;
	inline static const unsigned char COM_QUIT = 1;
	inline static const unsigned char COM_LOGIN = 2;
	inline static const unsigned char COM_USERLIST = 3;
	inline static const unsigned char COM_FILELIST = 4;
	inline static const unsigned char COM_MYFILES = 6;
	inline static const unsigned char COM_FGSERVE = 10;
	inline static const unsigned char COM_BGSERVE = 11;
	inline static const unsigned char COM_PUBLISH = 12;
	inline static const unsigned char COM_STOP_SERVER = 13;
	inline static const unsigned char COM_DOWNLOADFROM = 23;
	inline static const unsigned char COM_SEARCH = 24;
	inline static const unsigned char COM_DOWNLOAD = 25;
	inline static const unsigned char COM_LOGOUT = 30;
	inline static const unsigned char COM_SLEEP = 49;
	inline static const unsigned char COM_HELP = 50;
	inline static const unsigned char COM_SOCKET_IN = 100;


private:
	/**
	 * Códigos de los comandos válidos que puede
	 * introducir el usuario del shell. El orden
	 * es importante para relacionarlos con la cadena
	 * que debe introducir el usuario y con la ayuda
	 */
	inline static const std::vector<unsigned char> _valid_user_commands {
		COM_QUIT,
		COM_LOGIN,
		COM_USERLIST,
		COM_FILELIST,
		COM_MYFILES,
		COM_FGSERVE,
		COM_BGSERVE,
		COM_PUBLISH,
		COM_STOP_SERVER,
		COM_DOWNLOADFROM,
		COM_SEARCH,
		COM_DOWNLOAD,
		COM_LOGOUT,
		COM_SLEEP,
		COM_HELP,
		COM_SOCKET_IN
	};

	/**
	 * cadena exacta de cada orden
	 */
	inline static const std::vector<std::string> _valid_user_commands_str {
		"quit",
		"login",
		"userlist",	
		"filelist",
		"myfiles",
		"fgserve",
		"bgserve",
		"publish",
		"stopserver",
		"downloadfrom",
		"search",
		"download",
		"logout",
		"sleep",
		"help"
	};

	/**
	 * Mensaje de ayuda para cada orden
	 */
	inline static const std::vector<std::string> _valid_user_commands_help {
		"quit the application",
		"log into <directory> using <nickname>",
		"show list of users logged into the directory",
		"show list of files tracked by the directory",
		"show contents of local folder (files that may be served)",
		"run file server in foreground (blocking)",
		"run file server in background (non-blocking)",
		"publish list of served files to the directory",
		"stop file server running in background",
		"download from server given by <nickname> the file identified by <hash>",
		"show list of servers sharing the file identified by <hash>",
		"download the file identified by <hash> from all available server(s)",
		"log out from the current directory",
		"sleep during <num> seconds",
		"shows this information"
	};

public:
	/**
	 * Transforma una cadena introducida en el código de comando correspondiente
	 */
	static unsigned char stringToCommand(std::string comStr);

	static std::pair<bool, std::string> commandToString(unsigned char command);

	/**
	 * Imprime la lista de comandos y la ayuda de cada uno
	 */
	static void printCommandsHelp();
};