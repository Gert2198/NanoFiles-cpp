#pragma once

#include <thread>
#include <chrono>

#include "NFCommands.hpp"
#include "aplication/NanoFiles.hpp"

class NFShell {
	/**
	 * Scanner para leer comandos de usuario de la entrada estándar
	 */
	// private Scanner reader;

	unsigned char command = NFCommands::COM_INVALID;
	std::vector<std::string> commandArgs;

	bool enableComSocketIn = false;
	bool skipValidateArgs;

public:
	/*
	 * Testing-related: print command to stdout (when reading commands from stdin)
	 */
	inline static const std::string FILENAME_TEST_SHELL = ".nanofiles-test-shell";
	inline static bool verboseShell = false;

	NFShell();

	// devuelve el comando introducido por el usuario
	unsigned char getCommand();

	// Devuelve los parámetros proporcionados por el usuario para el comando actual
	std::vector<std::string> getCommandArguments();

	// Espera hasta obtener un comando válido entre los comandos existentes
	void readGeneralCommand();

	static void enableVerboseShell();
private:
	// Usa la entrada estándar para leer comandos y procesarlos
	std::vector<std::string> readGeneralCommandFromStdIn();

	// Algunos comandos requieren un parámetro
	// Este método comprueba si se proporciona parámetro para los comandos
	bool validateCommandArguments(std::vector<std::string> args);
};