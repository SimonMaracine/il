#pragma once

#include <iostream>

enum class TokenType {
    None,
    Int,
    Float,
    Plus,
    Minus,
    Mul,
    Div,
    LeftParen,
    RightParen
};

class Token {
private:
    union Value {
        int integer;
        float real;
    };
public:
    Token() = default;
    Token(TokenType type)
        : type(type) {}
    ~Token() = default;

    TokenType type = TokenType::None;
    Value value;
};

std::ostream& operator<<(std::ostream& stream, const Token& token);
