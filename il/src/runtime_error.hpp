#pragma once

#include <string>

#include "token.hpp"

struct RuntimeError {
    RuntimeError(const token::Token& token, const std::string& message)
        : token(token), message(message) {}

    token::Token token;
    std::string message;
};
