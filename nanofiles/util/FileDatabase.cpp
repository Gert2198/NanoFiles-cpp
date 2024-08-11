#include "FileDatabase.hpp"

FileDatabase::FileDatabase(std::string sharedFolder) {
    fs::directory_entry theDir(sharedFolder);
    if (!theDir.exists()) fs::create_directory(sharedFolder);
    files = FileInfo::loadFileMapFromFolder(fs::directory_entry(sharedFolder));
    if (files.size() == 0) 
        std::cerr << "*WARNING: No files found in folder " << sharedFolder << std::endl;
}

std::vector<FileInfo> FileDatabase::getFiles() {
    std::vector<FileInfo> fileinfoarray(files.size());
    std::map<std::string, FileInfo>::iterator it;
    int numFiles = 0;
    for (it = files.begin(); it != files.end(); it++) {
        fileinfoarray[numFiles++] = it->second;
    }
    return fileinfoarray;
}

std::string FileDatabase::lookupFilePath(std::string fileHash) {
    auto it = files.find(fileHash);

    if (it != files.end()) 
        return it->second.filePath;

    return {};
}