#pragma once

#include <string>
#include <cstddef>
#include <ostream>
#include <utility>
#include <variant>
#include <string_view>
#include <array>
#include <cassert>

namespace token {
    enum class TokenType {
        // Miscellaneous
        Eof,

        // Literals
        Identifier, String, Integer, Float,

        // Parentheses
        LeftParen, RightParen, LeftBrace, RightBrace,

        // Punctuation
        Comma, Dot, Semicolon,

        // Math operators
        Minus, Plus, Slash, Star,

        // Relational and logic operators
        BangEqual, Greater, GreaterEqual, Less, LessEqual, EqualEqual,

        // Keywords
        Let, True, False, None, Or, And, Not, If, Else, While, For, Fun, Return, Struct,

        // Other
        Equal,

        TokenCount
    };

    class Token {
    public:
        struct Empty {};
        using Literal = std::variant<Empty, std::string, long long, double>;

        Token(TokenType type, const std::string& lexeme, std::size_t line)
            : type(type), line(line), lexeme(lexeme) {}

        Token(TokenType type, const std::string& lexeme, std::size_t line, std::string&& value)
            : type(type), line(line), lexeme(lexeme), literal(std::move(value)) {
            assert(type == TokenType::String);
        }

        Token(TokenType type, const std::string& lexeme, std::size_t line, long long value)
            : type(type), line(line), lexeme(lexeme), literal(value) {
            assert(type == TokenType::Integer);
        }

        Token(TokenType type, const std::string& lexeme, std::size_t line, double value)
            : type(type), line(line), lexeme(lexeme), literal(value) {
            assert(type == TokenType::Float);
        }

        TokenType get_type() const { return type; };
        std::size_t get_line() const { return line; };
        const std::string& get_lexeme() const { return lexeme; }
        const Literal& get_literal() const { return literal; }
    private:
        TokenType type {};
        std::size_t line {};
        std::string lexeme;
        Literal literal;

        friend std::ostream& operator<<(std::ostream& stream, const Token& token);
    };

    using namespace std::string_view_literals;

    inline constexpr std::array TOKEN_NAMES {
        "Eof"sv,

        "Identifier"sv, "String"sv, "Integer"sv, "Float"sv,

        "LeftParen"sv, "RightParen"sv, "LeftBrace"sv, "RightBrace"sv,

        "Comma"sv, "Dot"sv, "Semicolon"sv,

        "Minus"sv, "Plus"sv, "Slash"sv, "Star"sv,

        "BangEqual"sv, "Greater"sv, "GreaterEqual"sv, "Less"sv, "LessEqual"sv, "EqualEqual"sv,

        "Let"sv, "True"sv, "False"sv, "None"sv, "Or"sv, "And"sv, "Not"sv, "If"sv, "Else"sv, "While"sv, "For"sv, "Fun"sv, "Return"sv, "Struct"sv,

        "Equal"sv
    };

    static_assert(std::size(TOKEN_NAMES) == static_cast<std::size_t>(TokenType::TokenCount));

    inline std::ostream& operator<<(std::ostream& stream, TokenType token_type) {
        stream << TOKEN_NAMES[static_cast<std::size_t>(token_type)];

        return stream;
    }

    inline std::ostream& operator<<(std::ostream& stream, const Token& token) {
        stream << token.type << ' ' << token.lexeme;

        switch (token.type) {
            case TokenType::String:
                stream << ' ' << std::get<1u>(token.literal);
                break;
            case TokenType::Integer:
                stream << ' ' << std::get<2u>(token.literal);
                break;
            case TokenType::Float:
                stream << ' ' << std::get<3u>(token.literal);
                break;
            default:
                break;
        }

        return stream;
    }
}
