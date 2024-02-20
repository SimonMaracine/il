#pragma once

#include <string>

#include "token.hpp"

struct RuntimeError {
    RuntimeError(const Token& token, const std::string& message)
        : token(token), message(message) {}

    Token token;
    std::string message;
};
