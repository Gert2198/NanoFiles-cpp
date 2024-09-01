#include "FileInfo.hpp"

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

#include <vector>

#include <fstream>

#include <algorithm>
#include <cctype>

std::string FileInfo::toString() {
    std::ostringstream strBuf;

    strBuf << std::left << std::setw(30) << fileName;
    strBuf << std::right << std::setw(10) << fileSize;
    strBuf << " " << std::left << std::setw(45) << fileHash;

    return strBuf.str();
}

void FileInfo::printToSysout(std::vector<FileInfo>  files) {
    std::ostringstream strBuf;

    strBuf << std::left << std::setw(30) << "Name";
    strBuf << std::right << std::setw(10) << "Size";
    strBuf << " " << std::left << std::setw(45) << "Hash";
    std::cout << strBuf.str() << std::endl;
    for (FileInfo file : files) 
        std::cout << file.toString() << std::endl;
}

std::vector<FileInfo> FileInfo::loadFilesFromFolder(std::string sharedFolderPath) {
    fs::directory_entry directory(sharedFolderPath);
    
    std::map<std::string, FileInfo> files = loadFileMapFromFolder(directory);

    files.size();

    std::vector<FileInfo> fileinfoarray(files.size());
    std::map<std::string, FileInfo>::iterator it;
    int numFiles = 0;
    for (it = files.begin(); it != files.end(); it++) {
        fileinfoarray[numFiles++] = it->second;
    }
    return fileinfoarray;
}

std::vector<FileInfo> FileInfo::lookupHashSubstring(std::vector<FileInfo> files, std::string hashSubstr) {
    std::string needle = hashSubstr;
    
    std::transform(needle.begin(), needle.end(), needle.begin(), [](unsigned char c){ return std::tolower(c); });

    
    std::vector<FileInfo> matchingFiles;
    for (int i = 0; i < files.size(); i++) {
        std::string hash = files[i].fileHash;
        std::transform(hash.begin(), hash.end(), hash.begin(), [](unsigned char c){ return std::tolower(c); });
        if (hash.find(needle) != std::string::npos) {
            matchingFiles.push_back(files[i]);
        }
    }
    
    return matchingFiles;
}

std::map<std::string, FileInfo> FileInfo::loadFileMapFromFolder(const fs::directory_entry& folder) {
    std::map<std::string, FileInfo> files;
    scanFolderRecursive(folder, files);
    return files;
}

std::vector<char> FileInfo::readFile(const std::string& file_name) {
    std::ifstream file(file_name, std::ios::binary);
    std::istream_iterator<char> start(file), end;
    std::vector<char> result(start, end);
    std::cout << "Read " << result.size() << " bytes" << std::endl;
    return result;
}

bool FileInfo::fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.is_open();  
}

std::string FileInfo::getAbsolutePath(const std::string& relative_path) {
    char absolute_path[MAX_PATH];
    if (GetFullPathNameA(relative_path.c_str(), MAX_PATH, absolute_path, nullptr)) {
        return std::string(absolute_path);
    } else {
        return std::string(); // Devuelve una cadena vacía si no se puede resolver la ruta
    }
}

bool FileInfo::canRead(const fs::path& p) {
    std::error_code ec; // For noexcept overload usage.
    auto perms = fs::status(p, ec).permissions();
    if ((perms & fs::perms::owner_read) != fs::perms::none &&
        (perms & fs::perms::group_read) != fs::perms::none &&
        (perms & fs::perms::others_read) != fs::perms::none)
    {
        return true;
    }
    return false;
}

void FileInfo::scanFolderRecursive(const fs::directory_entry& folder, std::map<std::string, FileInfo>& files) {
    if (!folder.exists()) {
        std::cerr << "scanFolder cannot find folder " << folder.path() << std::endl;
        return;
    }
    if (!canRead(folder.path())) {
        std::cerr << "scanFolder cannot access folder " << folder.path() << std::endl;
        return;
    }

    for (const auto & entry : fs::directory_iterator(folder)) {
        if (entry.is_directory()) scanFolderRecursive(entry, files);
        else {
            std::string fileName = entry.path().filename().string();
            std::string filePath = entry.path().string();
            std::string fileHash = FileDigest::computeFileChecksumString(filePath);
            long fileSize = entry.file_size();
            if (fileSize > 0) 
                files.insert(std::make_pair(fileHash, FileInfo(fileHash, fileName, fileSize, filePath)));
            else {
                // TODO: descomentar despues de crear NFShell

                // if (fileName == NFShell::FILENAME_TEST_SHELL) {
                // 	NFShell::enableVerboseShell();
                // 	std::cout << "[Enabling verbose shell]" << std::endl;
                // } else {
                // 	std::cout << "Ignoring empty file found in shared folder: " << filePath << std::endl;
                // }
            }
        }
    }
}