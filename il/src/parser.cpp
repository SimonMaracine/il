#include "parser.hpp"

bool Parser::match(std::initializer_list<token::TokenType> types) {
    for (const token::TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }

    return false;
}

bool Parser::check(token::TokenType type) {
    if (reached_end()) {
        return false;
    }

    return peek().get_type() == type;
}

const token::Token& Parser::advance() {
    if (!reached_end()) {
        current++;
    }

    return previous();
}

bool Parser::reached_end() {
    return peek().get_type() == token::TokenType::Eof;
}

const token::Token& Parser::peek() {
    return tokens[current];
}

const token::Token& Parser::previous() {
    return tokens[current - 1u];
}

const token::Token& Parser::consume(token::TokenType type, const std::string& message) {
    if (check(type)) {
        return advance();
    }

    throw error(peek(), message);
}

Parser::ParseError Parser::error(const token::Token& token, const std::string& message) {
    ctx->error(token, message);

    return ParseError();
}

void Parser::synchronize() {
    advance();

    while (!reached_end()) {
        if (previous().get_type() == token::TokenType::Semicolon) {
            return;
        }

        switch (peek().get_type()) {
            case token::TokenType::Let:  // TODO other
                return;
            default:
                break;
        }

        advance();
    }
}
