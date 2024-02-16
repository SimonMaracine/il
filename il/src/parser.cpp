#include "parser.hpp"

#include "context.hpp"

std::shared_ptr<Expr<int>> Parser::parse() {
    try {
        return expression();
    } catch (ParseError) {
        return nullptr;
    }
}

std::shared_ptr<Expr<int>> Parser::expression() {
    return equality();
}

std::shared_ptr<Expr<int>> Parser::equality() {
    std::shared_ptr<Expr<int>> expr {comparison()};

    while (match({TokenType::BangEqual, TokenType::EqualEqual})) {
        const Token& operator_ {previous()};
        std::shared_ptr<Expr<int>> right {comparison()};
        expr = std::make_shared<Binary<int>>(expr, operator_, right);
    }

    return expr;
}

std::shared_ptr<Expr<int>> Parser::comparison() {
    std::shared_ptr<Expr<int>> expr {term()};

    while (match({TokenType::Greater, TokenType::GreaterEqual, TokenType::Less, TokenType::LessEqual})) {
        const Token& operator_ {previous()};
        std::shared_ptr<Expr<int>> right {term()};
        expr = std::make_shared<Binary<int>>(expr, operator_, right);
    }

    return expr;
}

std::shared_ptr<Expr<int>> Parser::term() {
    std::shared_ptr<Expr<int>> expr {factor()};

    while (match({TokenType::Minus, TokenType::Plus})) {
        const Token& operator_ {previous()};
        std::shared_ptr<Expr<int>> right {factor()};
        expr = std::make_shared<Binary<int>>(expr, operator_, right);
    }

    return expr;
}

std::shared_ptr<Expr<int>> Parser::factor() {
    std::shared_ptr<Expr<int>> expr {unary()};

    while (match({TokenType::Slash, TokenType::Star})) {
        const Token& operator_ {previous()};
        std::shared_ptr<Expr<int>> right {unary()};
        expr = std::make_shared<Binary<int>>(expr, operator_, right);
    }

    return expr;
}

std::shared_ptr<Expr<int>> Parser::unary() {
    if (match({TokenType::Minus, TokenType::Bang})) {
        const Token& operator_ {previous()};
        std::shared_ptr<Expr<int>> right {unary()};
        return std::make_shared<Unary<int>>(operator_, right);
    }

    return primary();
}

std::shared_ptr<Expr<int>> Parser::primary() {
    if (match({TokenType::Number, TokenType::String})) {
        return std::make_shared<Literal<int>>(previous().get_literal());
    }

    if (match({TokenType::True})) {
        return std::make_shared<Literal<int>>(true);
    }

    if (match({TokenType::False})) {
        return std::make_shared<Literal<int>>(false);
    }

    if (match({TokenType::Null})) {
        return std::make_shared<Literal<int>>(literal::Null());
    }

    if (match({TokenType::LeftParen})) {
        std::shared_ptr<Expr<int>> expr {expression()};
        consume(TokenType::RightParen, "Expected `)` after expression");
        return std::make_shared<Grouping<int>>(expr);
    }

    throw error(peek(), "Expected an expression");
}

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
    ctx.error(token, message);
    return ParseError();
}

// TODO will be used later
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
