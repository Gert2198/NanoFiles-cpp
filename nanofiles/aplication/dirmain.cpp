#define _WIN32_WINNT 0x0A00
#include <winsock2.h>
#include <ws2tcpip.h>

#include <iostream>

#include "udp/server/NFDirectoryServer.hpp"

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

double DEFAULT_CORRUPTION_PROBABILITY = 0.0;

int main(int argc, char *argv[]) {
    double datagramCorruptionProbability = DEFAULT_CORRUPTION_PROBABILITY;

    std::string arg;
    if (argc > 1 && argv[1][0] == '-') {
        arg = argv[1];
        if (arg == "-loss") {
            if (argc == 3) {
                try {
                    datagramCorruptionProbability = std::stod(argv[2]);
                } catch (std::invalid_argument e) {
                    std::cerr << "Wrong value passed to option " << arg << std::endl;
                    return 1;
                }
            } else std::cerr << "option " << arg << " requires a value" << std::endl;
        } else std::cerr << "Illegal option " << arg << std::endl;
    }

    std::cout << "Probability of corruption for received datagrams: " << datagramCorruptionProbability << std::endl;

    WSADATA wsaData;
    if (!initWSAData(wsaData)) std::cerr << "WSA not initialized" << std::endl; 

    try {
        NFDirectoryServer dir(datagramCorruptionProbability);
        dir.run();
    } catch (std::exception e) {
        std::cerr << "Error starting Directory server: " << e.what() << std::endl;
        std::cerr << "Maybe directory cannot create UDP socket" << std::endl;
        std::cerr << "Most likely a Directory process is already running and listening on that port..." << std::endl;
        exit(-1);
    } 
}