#pragma once

#include <vector>
#include <string>
#include <cstddef>

#include "token.hpp"
#include "context.hpp"

class Scanner {
public:
    Scanner(const std::string& source_code, Context* ctx)
        : source_code(source_code), ctx(ctx) {}

    std::vector<token::Token> scan();
private:
    void scan_token();
    char advance();
    void add_token(token::TokenType type);
    void add_token(token::TokenType type, std::string&& value);
    void add_token(token::TokenType type, long long value);
    void add_token(token::TokenType type, double value);
    bool match(char character);
    char peek();
    char peek_two();
    bool reached_end();
    bool is_digit(char character);
    bool is_alpha(char character);
    bool is_alpha_numeric(char character);
    void string();
    void number();
    void identifier();
    long long parse_long_long(const std::string& string);
    double parse_double(const std::string& string);

    std::string source_code;
    std::vector<token::Token> tokens;

    std::size_t start {};  // First character in the lexeme currently scanned
    std::size_t current {};  // Current in source string
    std::size_t line {1u};  // Line of the current character

    Context* ctx {nullptr};
};
