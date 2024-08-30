#pragma once

#define _WIN32_WINNT 0x0A00
#include <winsock2.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>

#include "tcp/message/PeerMessage.hpp"  
#include "aplication/NanoFiles.hpp"
#include "util/FileInfo.hpp" 

class NFServerComm {
public:
    static void serveFilesToClient(SOCKET clientSocket, SOCKET serverSocket);
};