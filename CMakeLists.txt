cmake_minimum_required(VERSION 3.5)
project(SocketTests)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(client 
"nanofiles/aplication/nfmain.cpp"
"nanofiles/logic/NFController.cpp"
"nanofiles/shell/NFShell.cpp"
"nanofiles/shell/NFCommands.cpp"
"nanofiles/logic/NFControllerLogicDir.cpp"
"nanofiles/logic/NFControllerLogicP2P.cpp"
"nanofiles/udp/client/DirectoryConnector.cpp"
"nanofiles/udp/message/DirMessage.cpp"
"nanofiles/tcp/client/NFConnector.cpp"
"nanofiles/tcp/message/PeerMessage.cpp"
"nanofiles/tcp/message/PeerMessageOps.cpp"
"nanofiles/tcp/server/NFServer.cpp"
"nanofiles/tcp/server/NFServerComm.cpp"
"nanofiles/tcp/server/NFServerSimple.cpp"
"nanofiles/tcp/server/NFServerThread.cpp"
"nanofiles/util/FileDataBase.cpp"
"nanofiles/util/FileInfo.cpp"
"nanofiles/util/FileDigest.cpp"
"nanofiles/util/SocketManager.cpp"
"nanofiles/util/StringUtils.cpp"
"winsock/InetSocketAddress.cpp"
)

add_executable(server 
"nanofiles/aplication/dirmain.cpp"
"nanofiles/udp/server/NFDirectoryServer.cpp"
"nanofiles/udp/message/DirMessage.cpp"
"nanofiles/util/FileInfo.cpp"
"nanofiles/util/FileDigest.cpp"
"nanofiles/util/StringUtils.cpp"
"winsock/InetSocketAddress.cpp"
)

target_compile_features(server PRIVATE cxx_std_17)
target_compile_features(client PRIVATE cxx_std_17)

target_include_directories(server PRIVATE include)
target_include_directories(server PRIVATE nanofiles)
target_include_directories(server PRIVATE winsock)
target_include_directories(server PRIVATE include/vendor)

target_include_directories(client PRIVATE include)
target_include_directories(client PRIVATE nanofiles)
target_include_directories(client PRIVATE winsock)
target_include_directories(client PRIVATE include/vendor)

find_library(LIB_SSL ssl PATHS lib)
find_library(LIB_CRYPTO crypto PATHS lib)

target_link_libraries(server PRIVATE "${LIB_SSL}")
target_link_libraries(server PRIVATE "${LIB_CRYPTO}")

target_link_libraries(client PRIVATE "${LIB_SSL}")
target_link_libraries(client PRIVATE "${LIB_CRYPTO}")

target_link_libraries(server PRIVATE ws2_32)
target_link_libraries(client PRIVATE ws2_32)