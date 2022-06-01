#pragma once

#include <string>
#include <optional>
#include <vector>
#include <utility>

#include "token.h"

class Lexer {
public:
    Lexer(std::string&& text)
        : text(std::move(text)) {}
    ~Lexer() = default;

    std::vector<Token> tokenize();    
private:
    void advance();
    Token build_number();

    std::string text;
    size_t position = 0;
    std::optional<char> current_character = std::nullopt;
};
