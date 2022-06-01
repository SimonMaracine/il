#include <iostream>
#include <cassert>

#include "token.h"

std::ostream& operator<<(std::ostream& stream, const Token& token) {
    switch (token.type) {
        case TokenType::None:
            assert(false);
            break;
        case TokenType::Int:
            stream << "Int(" << token.value.integer << ")";
            break;
        case TokenType::Float:
            stream << "Float(" << token.value.real << ")";
            break;
        case TokenType::Plus:
            stream << "Plus";
            break;
        case TokenType::Minus:
            stream << "Minus";
            break;
        case TokenType::Mul:
            stream << "Mul";
            break;
        case TokenType::Div:
            stream << "Div";
            break;
        case TokenType::LeftParen:
            stream << "LeftParen";
            break;
        case TokenType::RightParen:
            stream << "RightParen";
            break;
        default:
            assert(false);           
    }

    return stream;
}
