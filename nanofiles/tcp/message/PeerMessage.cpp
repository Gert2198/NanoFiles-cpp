#include "PeerMessage.hpp"

PeerMessage::PeerMessage() {
    opcode = PeerMessageOps::OPCODE_INVALID_CODE;
}

PeerMessage::PeerMessage(unsigned char op) {
    opcode = op;
}


PeerMessage::PeerMessage(unsigned char op, unsigned char longitud, std::string hash) {
    opcode = op;
    hashLenght = longitud;
    fileHash = hash;
}

PeerMessage::PeerMessage(unsigned char op, bool ultimo, int longitud, std::vector<char> fichero) {
    opcode = op;
    this->last = ultimo;
    length = longitud;
    this->file = fichero;
}

unsigned char PeerMessage::getOpcode() {
    return opcode;
}

unsigned char PeerMessage::getHashLenght() {
    return hashLenght;
}

void PeerMessage::setHashLenght(unsigned char hashLenght) {
    assert(opcode == PeerMessageOps::OPCODE_DOWNLOAD_REQUEST);
    this->hashLenght = hashLenght;
}

std::string PeerMessage::getFileHash() {
    return fileHash;
}

void PeerMessage::setFileHash(std::string fileHash) {
    assert(opcode == PeerMessageOps::OPCODE_DOWNLOAD_REQUEST);
    this->fileHash = fileHash;
}


bool PeerMessage::isLast() {
    return last;
}

void PeerMessage::setLast(bool last) {
    assert(opcode == PeerMessageOps::OPCODE_DOWNLOAD_RESPONSE);
    this->last = last;
}

int PeerMessage::getLength() {
    return length;
}

void PeerMessage::setLength(int length) {
    assert(opcode == PeerMessageOps::OPCODE_DOWNLOAD_RESPONSE);
    this->length = length;
}

std::vector<char> PeerMessage::getFile() {
    return file;
}

void PeerMessage::setFile(std::vector<char> file) {
    assert(opcode == PeerMessageOps::OPCODE_DOWNLOAD_RESPONSE);
    this->file = file;
}

PeerMessage PeerMessage::readMessageFromInputStream(SocketManager dis) { // throws IOException
    /*
    * En función del tipo de mensaje, leer del socket a través del "dis" el
    * resto de campos para ir extrayendo con los valores y establecer los atributos
    * del un objeto PeerMessage que contendrá toda la información del mensaje, y que
    * será devuelto como resultado. NOTA: Usar dis.readFully para leer un array de
    * bytes, dis.readInt para leer un entero, etc.
    */
    
    PeerMessage message;
    char opcode = dis.readByte(); 
    switch (opcode) {
    case PeerMessageOps::OPCODE_DOWNLOAD_REQUEST: {
        char hashLenght = dis.readByte(); 
        
        char * data = new char[hashLenght];
        dis.readFully(data, hashLenght);
        std::string hash(data);
        delete[] data;
        
        message = PeerMessage(opcode, hashLenght, hash);
        break;
    }
    case PeerMessageOps::OPCODE_DOWNLOAD_RESPONSE: {
        bool esfinal = dis.readBool();
        int longitud = dis.readInt();
        char * ficheroOhash = new char[longitud];
        dis.readFully(ficheroOhash, longitud);

        std::vector<char> data(ficheroOhash, ficheroOhash + longitud);
        delete[] ficheroOhash;
        
        message = PeerMessage(opcode, esfinal, longitud, data);
        break;
    }
    case PeerMessageOps::OPCODE_FILE_NOT_FOUND:
    case PeerMessageOps::OPCODE_AMBIGUOUS_HASH:
        message = PeerMessage(opcode);
        break;
        
    default:
        std::cerr << "PeerMessage.readMessageFromInputStream doesn't know how to parse this message opcode: "
                << opcode << " = " << PeerMessageOps::opcodeToOperation(opcode).second << std::endl;
        exit(-1);
    }
    return message;
}

void PeerMessage::writeMessageToOutputStream(SocketManager dos) { // throws IOException
    /*
    * Escribir los bytes en los que se codifica el mensaje en el socket a
    * través del "dos", teniendo en cuenta opcode del mensaje del que se trata y
    * los campos relevantes en cada caso. NOTA: Usar dos.write para leer un array
    * de bytes, dos.writeInt para escribir un entero, etc.
    */

    dos.writeByte(opcode);
    switch (opcode) {
    case PeerMessageOps::OPCODE_DOWNLOAD_REQUEST: {
        assert(hashLenght > 0 && fileHash.length() == hashLenght);

        dos.writeByte((char)hashLenght);
        const char * hashValue = fileHash.c_str();
        dos.write(hashValue);
        break;
    }
    case PeerMessageOps::OPCODE_DOWNLOAD_RESPONSE: {
        assert(length > 0 && length <= 32768 && file.size() == length);
        
        dos.writeBool(last);
        dos.writeInt(length);
        dos.write(file);
        break;
    }
    case PeerMessageOps::OPCODE_FILE_NOT_FOUND:
    case PeerMessageOps::OPCODE_AMBIGUOUS_HASH:
        break;

    default:
        std::cerr << "PeerMessage.writeMessageToOutputStream found unexpected message opcode " << opcode << "("
                << PeerMessageOps::opcodeToOperation(opcode).second << ")" << std::endl;
    }
}