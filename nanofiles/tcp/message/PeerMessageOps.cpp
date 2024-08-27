#include "PeerMessageOps.hpp"

std::pair<bool, unsigned char> PeerMessageOps::operationToOpcode(std::string opStr) {
    StringUtils::toLower(opStr);
    if (_operation_to_opcode.find(opStr) == _operation_to_opcode.end()) return {false, 0};
    return {true, _operation_to_opcode[opStr]};
}

/**
 * Transforma un opcode en la cadena correspondiente
 */
std::pair<bool, std::string> PeerMessageOps::opcodeToOperation(unsigned char opcode) {
    if (_opcode_to_operation.find(opcode) == _opcode_to_operation.end()) return {false, ""};
    return {true, _opcode_to_operation[opcode]};
}