#pragma once

enum class TokenType {
    None,
    Int,
    Float,
    Plus,
    Minus,
    Mul,
    Div,
    LParen,
    RParen
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
