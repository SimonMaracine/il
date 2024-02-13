#pragma once

#include <string>
#include <cstddef>
#include <ostream>
#include <variant>
#include <utility>

enum class TokenType {
    // Miscellaneous
    None,
    Eof,

    // Literals
    Identifier, String, Number,

    // Parentheses
    LeftParen, RightParen, LeftBrace, RightBrace,

    // Punctuation
    Comma, Dot, Semicolon,

    // Math operators
    Minus, Plus, Slash, Star,

    // Relational operators
    Bang, BangEqual, Greater, GreaterEqual, Less, LessEqual, Equal, EqualEqual

    // Keywords
    // TODO
};

class Token {
public:
    Token(TokenType type, const std::string& lexeme, std::size_t line)
        : type(type), line(line), lexeme(lexeme) {}

    Token(TokenType type, const std::string& lexeme, std::size_t line, std::string&& value)
        : type(type), line(line), lexeme(lexeme), literal(std::move(value)) {}

    Token(TokenType type, const std::string& lexeme, std::size_t line, double value)
        : type(type), line(line), lexeme(lexeme), literal(value) {}
private:
    TokenType type {};
    std::size_t line {};
    std::string lexeme;
    std::variant<std::string, double> literal;

    friend std::ostream& operator<<(std::ostream& stream, const Token& token);
};

inline std::ostream& operator<<(std::ostream& stream, const TokenType token_type) {
    switch (token_type) {
        case TokenType::None:
            stream << "None";
            break;
        case TokenType::Eof:
            stream << "Eof";
            break;
        case TokenType::Identifier:
            stream << "Identifier";
            break;
        case TokenType::String:
            stream << "String";
            break;
        case TokenType::Number:
            stream << "Number";
            break;
        case TokenType::LeftParen:
            stream << "LeftParen";
            break;
        case TokenType::RightParen:
            stream << "RightParen";
            break;
        case TokenType::LeftBrace:
            stream << "LeftBrace";
            break;
        case TokenType::RightBrace:
            stream << "RightBrace";
            break;
        case TokenType::Comma:
            stream << "Comma";
            break;
        case TokenType::Dot:
            stream << "Dot";
            break;
        case TokenType::Semicolon:
            stream << "Semicolon";
            break;
        case TokenType::Minus:
            stream << "Minus";
            break;
        case TokenType::Plus:
            stream << "Plus";
            break;
        case TokenType::Slash:
            stream << "Slash";
            break;
        case TokenType::Star:
            stream << "Star";
            break;
        case TokenType::Bang:
            stream << "Bang";
            break;
        case TokenType::BangEqual:
            stream << "BangEqual";
            break;
        case TokenType::Greater:
            stream << "Greater";
            break;
        case TokenType::GreaterEqual:
            stream << "GreaterEqual";
            break;
        case TokenType::Less:
            stream << "Less";
            break;
        case TokenType::LessEqual:
            stream << "LessEqual";
            break;
    }

    return stream;
}

inline std::ostream& operator<<(std::ostream& stream, const Token& token) {
    stream << token.type << ' ' << token.lexeme;

    switch (token.type) {
        case TokenType::String:
            stream << ' ' << std::get<0u>(token.literal);
            break;
        default:
            break;
    }

    return stream;
}