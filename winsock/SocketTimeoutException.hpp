#pragma once

#include <exception>
#include <string>

class SocketTimeoutException : public std::exception {
public:
    SocketTimeoutException(const std::string& message) : msg(message) {}

    const char* what() const noexcept override {
        return msg.c_str();
    }

private:
    std::string msg;
};