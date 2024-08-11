#pragma once

#include "FileInfo.hpp"

class FileDatabase {
private: 
    std::map<std::string, FileInfo> files;
public:
	FileDatabase(std::string sharedFolder);
	std::vector<FileInfo> getFiles();
	std::string lookupFilePath(std::string fileHash);
};