#pragma once

#include <memory>
#include <fstream>
#include <filesystem>
#include <cstdio>

#define _WIN32_WINNT 0x0A00
#include <winsock2.h>
#include <ws2tcpip.h>
#include "util/SocketManager.hpp"
#include "InetSocketAddress.hpp"
#include "tcp/message/PeerMessage.hpp"
#include "util/FileDigest.hpp"
#include "util/FileInfo.hpp"
#include "ServerSocket.hpp"

class NFConnector {
private:
    InetSocketAddress serverAddress;
    SOCKET clientSocket;
public:
    NFConnector(InetSocketAddress fserverAddr); // throws UnknownHostException, IOException

    bool downloadFile(const std::string& targetFileHashSubstr, std::fstream& file, const std::string& filePath); // throws IOException
	// void disconnect();
	InetSocketAddress getServerAddr();
};