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
    std::vector<std::shared_ptr<ast::stmt::Stmt<R>>> parse() {
        std::vector<std::shared_ptr<ast::stmt::Stmt<R>>> statements;

        while (!reached_end()) {
            statements.push_back(declaration<R>());
        }

        return statements;
    }
private:
    using ParseError = int;

    template<typename R>
    std::shared_ptr<ast::stmt::Stmt<R>> declaration() {
        try {
            if (match({TokenType::Let})) {
                return var_statement<R>();
            }

            return statement<R>();
        } catch (ParseError) {
            synchronize();

            return nullptr;
        }
    }

    template<typename R>
    std::shared_ptr<ast::stmt::Stmt<R>> statement() {
        if (match({TokenType::Print})) {
            return print_statement<R>();
        }

        if (match({TokenType::LeftBrace})) {
            return std::make_shared<ast::stmt::Block<R>>(block<R>());
        }

        if (match({TokenType::If})) {
            return if_statement<R>();
        }

        if (match({TokenType::While})) {
            return while_statement<R>();
        }

        return expr_statement<R>();
    }

    template<typename R>
    std::shared_ptr<ast::stmt::Stmt<R>> var_statement() {
        const Token& name {consume(TokenType::Identifier, "Expected a variable name")};

        std::shared_ptr<ast::expr::Expr<R>> initializer;

        if (match({TokenType::Equal})) {
            initializer = expression<R>();
        }

        consume(TokenType::Semicolon, "Expected `;` after variable declaration");

        return std::make_shared<ast::stmt::Let<R>>(name, initializer);
    }

    template<typename R>
    std::shared_ptr<ast::stmt::Stmt<R>> expr_statement() {
        std::shared_ptr<ast::expr::Expr<R>> expr {expression<R>()};

        consume(TokenType::Semicolon, "Expected `;` after expression");

        return std::make_shared<ast::stmt::Expression<R>>(expr);
    }

    template<typename R>
    std::shared_ptr<ast::stmt::Stmt<R>> print_statement() {
        std::shared_ptr<ast::expr::Expr<R>> expr {expression<R>()};

        consume(TokenType::Semicolon, "Expected `;` after expression");

        return std::make_shared<ast::stmt::Print<R>>(expr);
    }

    template<typename R>
    std::shared_ptr<ast::stmt::Stmt<R>> if_statement() {
        consume(TokenType::LeftParen, "Expected `(` after `if`");

        std::shared_ptr<ast::expr::Expr<R>> condition {expression<R>()};

        consume(TokenType::RightParen, "Expected `)` after if condition");

        std::shared_ptr<ast::stmt::Stmt<R>> then_branch {statement<R>()};
        std::shared_ptr<ast::stmt::Stmt<R>> else_branch;

        if (match({TokenType::Else})) {
            else_branch = statement<R>();
        }

        return std::make_shared<ast::stmt::If<R>>(condition, then_branch, else_branch);
    }

    template<typename R>
    std::shared_ptr<ast::stmt::Stmt<R>> while_statement() {
        consume(TokenType::LeftParen, "Expected `(` after `if`");

        std::shared_ptr<ast::expr::Expr<R>> condition {expression<R>()};

        consume(TokenType::RightParen, "Expected `)` after if condition");

        std::shared_ptr<ast::stmt::Stmt<R>> body {statement<R>()};

        return std::make_shared<ast::stmt::While<R>>(condition, body);
    }

    template<typename R>
    std::vector<std::shared_ptr<ast::stmt::Stmt<R>>> block() {
        std::vector<std::shared_ptr<ast::stmt::Stmt<R>>> statements;

        while (!check(TokenType::RightBrace) && !reached_end()) {
            statements.push_back(declaration<R>());
        }

        consume(TokenType::RightBrace, "Excpected `}` after block");

        return statements;
    }

    template<typename R>
    std::shared_ptr<ast::expr::Expr<R>> expression() {
        return assignment<R>();
    }

    template<typename R>
    std::shared_ptr<ast::expr::Expr<R>> assignment() {
        std::shared_ptr<ast::expr::Expr<R>> expr {logic_or<R>()};

        if (match({TokenType::Equal})) {
            const Token& equals {previous()};
            std::shared_ptr<ast::expr::Expr<R>> value {assignment<R>()};  // Recursively parse assignments

            // Check if left hand side is an l-value
            auto variable {std::dynamic_pointer_cast<ast::expr::Variable<R>>(expr)};

            if (variable != nullptr) {
                return std::make_shared<ast::expr::Assignment<R>>(variable->name, value);
            }

            error(equals, "Invalid assignment target");  // Don't enter panic mode
        }

        return expr;
    }

    template<typename R>
    std::shared_ptr<ast::expr::Expr<R>> logic_or() {
        std::shared_ptr<ast::expr::Expr<R>> expr {logic_and<R>()};

        while (match({TokenType::Or})) {
            const Token& operator_ {previous()};
            std::shared_ptr<ast::expr::Expr<R>> right {logic_and<R>()};
            expr = std::make_shared<ast::expr::Logical<R>>(expr, operator_, right);
        }

        return expr;
    }

    template<typename R>
    std::shared_ptr<ast::expr::Expr<R>> logic_and() {
        std::shared_ptr<ast::expr::Expr<R>> expr {equality<R>()};

        while (match({TokenType::And})) {
            const Token& operator_ {previous()};
            std::shared_ptr<ast::expr::Expr<R>> right {equality<R>()};
            expr = std::make_shared<ast::expr::Logical<R>>(expr, operator_, right);
        }

        return expr;
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

        if (match({TokenType::Identifier})) {
            return std::make_shared<ast::expr::Variable<R>>(previous());
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
