#pragma once

#include <string>
#include <vector>
#include <map>

#include "util/StringUtils.hpp"

class PeerMessageOps {
public:
    inline static const unsigned char OPCODE_INVALID_CODE = 0;
	inline static const unsigned char OPCODE_DOWNLOAD_REQUEST = 1; // codigo de descarga de ficheros
	inline static const unsigned char OPCODE_DOWNLOAD_RESPONSE = 2; // el fichero se devuelve bien
	inline static const unsigned char OPCODE_FILE_NOT_FOUND = 3; // el mensaje de "Oye, fichero no encontrado"
	inline static const unsigned char OPCODE_AMBIGUOUS_HASH = 4; // el mensaje de "Oye, fichero no encontrado"

private:
    inline static const std::vector<unsigned char> _valid_opcodes = {
			OPCODE_INVALID_CODE,
			OPCODE_DOWNLOAD_REQUEST, 
			OPCODE_DOWNLOAD_RESPONSE, 
			OPCODE_FILE_NOT_FOUND,
			OPCODE_AMBIGUOUS_HASH
    };
	inline static const std::vector<std::string> _valid_operations_str = {
        "invalidOpCode",
        "downloadRequest", 
        "downloadResponse", 
        "fileNotFound", 
        "ambiguousHash"
    };

	inline static std::map<std::string, unsigned char> _operation_to_opcode = {
        {"invalidopcode", OPCODE_INVALID_CODE},
        {"downloadrequest", OPCODE_DOWNLOAD_REQUEST},
        {"downloadresponse", OPCODE_DOWNLOAD_RESPONSE},
        {"filenotfound", OPCODE_FILE_NOT_FOUND},
        {"ambiguoushash", OPCODE_AMBIGUOUS_HASH}
    };
	inline static std::map<unsigned char, std::string> _opcode_to_operation {
        {OPCODE_INVALID_CODE, "invalidOpCode"},
        {OPCODE_DOWNLOAD_REQUEST, "downloadRequest"},
        {OPCODE_DOWNLOAD_RESPONSE, "downloadResponse"},
        {OPCODE_FILE_NOT_FOUND, "fileNotFound"},
        {OPCODE_AMBIGUOUS_HASH, "ambiguousHash"}
    };

public:
    /**
	 * Transforma una cadena en el opcode correspondiente
	 */
	static std::pair<bool, unsigned char> operationToOpcode(std::string opStr) {
        StringUtils::toLower(opStr);
        if (_operation_to_opcode.find(opStr) == _operation_to_opcode.end()) return {false, 0};
		return {true, _operation_to_opcode[opStr]};
	}

	/**
	 * Transforma un opcode en la cadena correspondiente
	 */
	static std::pair<bool, std::string> opcodeToOperation(unsigned char opcode) {
        if (_opcode_to_operation.find(opcode) == _opcode_to_operation.end()) return {false, ""};
		return {true, _opcode_to_operation[opcode]};
	}

};