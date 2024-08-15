#pragma once

#include <map>
#include <fstream>
#include <iterator>
#include <filesystem>
namespace fs = std::filesystem;

#include "FileDigest.hpp"

class FileInfo {
public:
    std::string fileHash;
	std::string fileName;
	std::string filePath;
	long fileSize = -1;

	FileInfo(std::string hash, std::string name, long size, std::string path) 
		: fileHash(hash), fileName(name), filePath(path), fileSize(size) {}
	FileInfo() = default;

	inline bool operator==(FileInfo obj) { return fileHash == obj.fileHash; }

	std::string toString();
	static void printToSysout(std::vector<FileInfo>  files);

	/**
	 * Scans the given directory and returns an array of FileInfo objects, one for
	 * each file recursively found in the given folder and its subdirectories.
	 * 
	 * @param sharedFolderPath The folder to be scanned
	 * @return An array of file metadata (FileInfo) of all the files found
	 */
	static std::vector<FileInfo> loadFilesFromFolder(std::string sharedFolderPath);
    static std::vector<FileInfo> lookupHashSubstring(std::vector<FileInfo> files, std::string hashSubstr);

	/**
	 * Scans the given directory and returns a map of <filehash,FileInfo> pairs.
	 * 
	 * @param folder The folder to be scanned
	 * @return A map of the metadata (FileInfo) of all the files recursively found
	 *         in the given folder and its subdirectories.
	 */
	static std::map<std::string, FileInfo> loadFileMapFromFolder(const fs::directory_entry folder);

	static std::vector<char> readFile(const std::string& file_name);


	bool operator<(const FileInfo& other) const {
        // Primero comparar por ruta
        if (filePath < other.filePath) return true;
        if (filePath > other.filePath) return false;

        // Si las rutas son iguales, comparar por hash
        if (fileHash < other.fileHash) return true;
        if (fileHash > other.fileHash) return false;

        // Si ambos, ruta y hash, son iguales, comparar por longitud
        return fileSize < other.fileSize;
    }

private:
    static bool canRead(const fs::path& p);
	static void scanFolderRecursive(const fs::directory_entry folder, std::map<std::string, FileInfo> files);	
};