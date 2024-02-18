#pragma once

#include <vector>
#include <cstddef>
#include <memory>
#include <initializer_list>
#include <string>

#include "token.hpp"
#include "ast.hpp"
#include "context.hpp"

class Parser {
public:
    Parser(const std::vector<Token>& tokens, Context* ctx)
        : tokens(tokens), ctx(ctx) {}

    template<typename R>
    std::shared_ptr<ast::expr::Expr<R>> parse() {
        try {
            return expression<R>();
        } catch (ParseError) {
            return nullptr;
        }
    }
private:
    using ParseError = int;

    template<typename R>
    std::shared_ptr<ast::expr::Expr<R>> expression() {
        return equality<R>();
    }

    template<typename R>
    std::shared_ptr<ast::expr::Expr<R>> equality() {
        std::shared_ptr<ast::expr::Expr<R>> expr {comparison<R>()};

        while (match({TokenType::BangEqual, TokenType::EqualEqual})) {
            const Token& operator_ {previous()};
            std::shared_ptr<ast::expr::Expr<R>> right {comparison<R>()};
            expr = std::make_shared<ast::expr::Binary<R>>(expr, operator_, right);
        }

        return expr;
    }

    template<typename R>
    std::shared_ptr<ast::expr::Expr<R>> comparison() {
        std::shared_ptr<ast::expr::Expr<R>> expr {term<R>()};

        while (match({TokenType::Greater, TokenType::GreaterEqual, TokenType::Less, TokenType::LessEqual})) {
            const Token& operator_ {previous()};
            std::shared_ptr<ast::expr::Expr<R>> right {term<R>()};
            expr = std::make_shared<ast::expr::Binary<R>>(expr, operator_, right);
        }

        return expr;
    }

    template<typename R>
    std::shared_ptr<ast::expr::Expr<R>> term() {
        std::shared_ptr<ast::expr::Expr<R>> expr {factor<R>()};

        while (match({TokenType::Minus, TokenType::Plus})) {
            const Token& operator_ {previous()};
            std::shared_ptr<ast::expr::Expr<R>> right {factor<R>()};
            expr = std::make_shared<ast::expr::Binary<R>>(expr, operator_, right);
        }

        return expr;
    }

    template<typename R>
    std::shared_ptr<ast::expr::Expr<R>> factor() {
        std::shared_ptr<ast::expr::Expr<R>> expr {unary<R>()};

        while (match({TokenType::Slash, TokenType::Star})) {
            const Token& operator_ {previous()};
            std::shared_ptr<ast::expr::Expr<R>> right {unary<R>()};
            expr = std::make_shared<ast::expr::Binary<R>>(expr, operator_, right);
        }

        return expr;
    }

    template<typename R>
    std::shared_ptr<ast::expr::Expr<R>> unary() {
        if (match({TokenType::Minus, TokenType::Bang})) {
            const Token& operator_ {previous()};
            std::shared_ptr<ast::expr::Expr<R>> right {unary<R>()};
            return std::make_shared<ast::expr::Unary<R>>(operator_, right);
        }

        return primary<R>();
    }

    template<typename R>
    std::shared_ptr<ast::expr::Expr<R>> primary() {
        if (match({TokenType::Number, TokenType::String})) {
            return std::make_shared<ast::expr::Literal<R>>(previous().get_literal());
        }

        if (match({TokenType::True})) {
            return std::make_shared<ast::expr::Literal<R>>(true);
        }

        if (match({TokenType::False})) {
            return std::make_shared<ast::expr::Literal<R>>(false);
        }

        if (match({TokenType::Null})) {
            return std::make_shared<ast::expr::Literal<R>>(literal::Null());
        }

        if (match({TokenType::LeftParen})) {
            std::shared_ptr<ast::expr::Expr<R>> expr {expression<R>()};
            consume(TokenType::RightParen, "Expected `)` after expression");
            return std::make_shared<ast::expr::Grouping<R>>(expr);
        }

        throw error(peek(), "Expected an expression");
    }

    bool match(std::initializer_list<TokenType> types);
    bool check(TokenType type);
    const Token& advance();
    bool reached_end();
    const Token& peek();
    const Token& previous();
    const Token& consume(TokenType type, const std::string& message);
    ParseError error(const Token& token, const std::string& message);
    void synchronize();

    std::vector<Token> tokens;
    std::size_t current {};

    Context* ctx {nullptr};
};
