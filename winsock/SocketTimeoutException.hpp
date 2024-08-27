#pragma once

#include <exception>
#include <string>

class SocketTimeoutException : public std::runtime_error {
public:
    SocketTimeoutException(const std::string& message)
        : std::runtime_error(message) {}
};