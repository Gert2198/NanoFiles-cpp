#include "FileDigest.hpp"

int FileDigest::getDigestSize(std::string algorithm) {
    if (algorithm == "SHA-1") 
        return SHA_DIGEST_LENGTH;
    throw std::runtime_error("NoSuchAlgorithmException");
}

std::vector<unsigned char> FileDigest::computeFileChecksum(std::string filename) {
    std::ifstream file(filename, std::ifstream::binary);
    if (!file.is_open()) {
        std::cerr << "FileNotFoundException" << std::endl;
        return {};
    }

    SHA_CTX shaContext;
    if (!SHA1_Init(&shaContext)) {
        std::cerr << "SHA1_Init failed" << std::endl;
        return {};
    }

    char buffer[4096];
    while (file.read(buffer, sizeof(buffer))) {
        if (!SHA1_Update(&shaContext, buffer, file.gcount())) {
            std::cerr << "SHA1_Update failed" << std::endl;
            return {};
        }
    }
    // Process the last block of data if necessary
    if (file.gcount() > 0) {
        if (!SHA1_Update(&shaContext, buffer, file.gcount())) {
            std::cerr << "SHA1_Update failed" << std::endl;
            return {};
        }
    }

    std::vector<unsigned char> digest(SHA_DIGEST_LENGTH);
    if (!SHA1_Final(digest.data(), &shaContext)) {
        std::cerr << "SHA1_Final failed" << std::endl;
        return {};
    }

    return digest;
}

std::string FileDigest::getChecksumHexString(std::vector<unsigned char> digest) {
    std::ostringstream hexStream;
    for (const auto& byte : digest) {
        hexStream << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(byte);
    }
    return hexStream.str();
}

int FileDigest::getFileDigestSize() {
    try {
        return getDigestSize(algorithm);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 0;
    }
}

std::string FileDigest::computeFileChecksumString(const std::string& filename) {
    return getChecksumHexString(computeFileChecksum(filename));
}