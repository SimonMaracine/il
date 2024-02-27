#include "context.hpp"

#include <iostream>

void Context::error(const token::Token& token, const std::string& message) {
    if (token.get_type() == token::TokenType::Eof) {
        report(token.get_line(), " at end", message);
    } else {
        report(token.get_line(), " at `" + token.get_lexeme() + "`", message);
    }
}

void Context::error(std::size_t line, const std::string& message) {
    report(line, "", message);
}

void Context::runtime_error(const token::Token& token, const std::string& message) {
    std::cerr << message + "\n[line " + std::to_string(token.get_line()) + "]\n";
    had_runtime_error = true;
}

void Context::report(std::size_t line, const std::string& where, const std::string& message) {
    std::cerr << "[line " << line << "] Error" << where << ": " << message << '\n';
    had_error = true;
}
