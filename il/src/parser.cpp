#include "parser.hpp"

bool Parser::match(std::initializer_list<TokenType> types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }

    return false;
}

bool Parser::check(TokenType type) {
    if (reached_end()) {
        return false;
    }

    return peek().get_type() == type;
}

const Token& Parser::advance() {
    if (!reached_end()) {
        current++;
    }

    return previous();
}

bool Parser::reached_end() {
    return peek().get_type() == TokenType::Eof;
}

const Token& Parser::peek() {
    return tokens[current];
}

const Token& Parser::previous() {
    return tokens[current - 1u];
}

const Token& Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        return advance();
    }

    throw error(peek(), message);
}

Parser::ParseError Parser::error(const Token& token, const std::string& message) {
    ctx->error(token, message);
    return ParseError();
}

void Parser::synchronize() {
    advance();

    while (!reached_end()) {
        if (previous().get_type() == TokenType::Semicolon) {
            return;
        }

        switch (peek().get_type()) {
            case TokenType::Let:  // TODO other
                return;
            default:
                break;
        }

        advance();
    }
}
