#include "context.hpp"

#include <iostream>

void Context::report(std::size_t line, const std::string& where, const std::string& message) {
    std::cout << "[line " << line << "] Error" << where << ": " << message;
    had_error = true;
}

void Context::error(std::size_t line, const std::string& message) {
    report(line, "", message);
}
