#pragma once

#include <vector>
#include <cstddef>
#include <memory>
#include <initializer_list>
#include <string>
#include <cassert>

#include "token.hpp"
#include "ast.hpp"
#include "context.hpp"
#include "object.hpp"

class Parser {
public:
    Parser(const std::vector<token::Token>& tokens, Context* ctx)
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
            if (match({token::TokenType::Let})) {
                return var_declaration<R>();
            }

            if (match({token::TokenType::Fun})) {
                return fun_declaration<R>();
            }

            if (match({token::TokenType::Struct})) {
                return struct_declaration<R>();
            }

            return statement<R>();
        } catch (ParseError) {
            synchronize();

            return nullptr;
        }
    }

    template<typename R>
    std::shared_ptr<ast::stmt::Stmt<R>> statement() {
        if (match({token::TokenType::LeftBrace})) {
            return std::make_shared<ast::stmt::Block<R>>(block<R>());
        }

        if (match({token::TokenType::If})) {
            return if_statement<R>();
        }

        if (match({token::TokenType::While})) {
            return while_statement<R>();
        }

        if (match({token::TokenType::For})) {
            return for_statement<R>();
        }

        if (match({token::TokenType::Return})) {
            return return_statement<R>();
        }

        return expr_statement<R>();
    }

    template<typename R>
    std::shared_ptr<ast::stmt::Stmt<R>> var_declaration() {
        const token::Token& name {consume(token::TokenType::Identifier, "Expected a variable name")};

        std::shared_ptr<ast::expr::Expr<R>> initializer;

        if (match({token::TokenType::Equal})) {
            initializer = expression<R>();
        }

        consume(token::TokenType::Semicolon, "Expected `;` after variable declaration");

        return std::make_shared<ast::stmt::Let<R>>(name, initializer);
    }

    template<typename R>
    std::shared_ptr<ast::stmt::Stmt<R>> fun_declaration() {
        return function<R>();
    }

    template<typename R>
    std::shared_ptr<ast::stmt::Stmt<R>> function() {
        const token::Token& name {consume(token::TokenType::Identifier, "Expected a function name")};

        consume(token::TokenType::LeftParen, "Expected `(` after function name");

        std::vector<token::Token> parameters;

        if (!check(token::TokenType::RightParen)) {
            do {
                if (parameters.size() >= 255u) {
                    error(peek(), "Too many parameters (255 maximum)");  // Don't go in panic mode
                }

                parameters.push_back(consume(token::TokenType::Identifier, "Expected a parameter name"));
            } while (match({token::TokenType::Comma}));
        }

        consume(token::TokenType::RightParen, "Expected `)` after function parameters");

        consume(token::TokenType::LeftBrace, "Expected `{` before function body");

        const std::vector<std::shared_ptr<ast::stmt::Stmt<R>>> body {block<R>()};

        return std::make_shared<ast::stmt::Function<R>>(name, parameters, body);
    }

    template<typename R>
    std::shared_ptr<ast::stmt::Stmt<R>> struct_declaration() {
        const token::Token& name {consume(token::TokenType::Identifier, "Expected a struct name")};

        consume(token::TokenType::LeftBrace, "Expected `{` before struct body");

        std::vector<std::shared_ptr<ast::stmt::Function<R>>> methods;

        while (!check(token::TokenType::RightBrace) && !reached_end()) {
            methods.push_back(std::static_pointer_cast<ast::stmt::Function<R>>(function<R>()));
        }

        consume(token::TokenType::RightBrace, "Expected `}` after struct body");

        return std::make_shared<ast::stmt::Struct<R>>(name, methods);
    }

    template<typename R>
    std::shared_ptr<ast::stmt::Stmt<R>> expr_statement() {
        std::shared_ptr<ast::expr::Expr<R>> expr {expression<R>()};

        consume(token::TokenType::Semicolon, "Expected `;` after expression");

        return std::make_shared<ast::stmt::Expression<R>>(expr);
    }

    template<typename R>
    std::shared_ptr<ast::stmt::Stmt<R>> if_statement() {
        const token::Token& paren {consume(token::TokenType::LeftParen, "Expected `(` after `if`")};

        std::shared_ptr<ast::expr::Expr<R>> condition {expression<R>()};

        consume(token::TokenType::RightParen, "Expected `)` after if condition");

        std::shared_ptr<ast::stmt::Stmt<R>> then_branch {statement<R>()};
        std::shared_ptr<ast::stmt::Stmt<R>> else_branch;

        if (match({token::TokenType::Else})) {
            else_branch = statement<R>();
        }

        return std::make_shared<ast::stmt::If<R>>(condition, then_branch, else_branch, paren);
    }

    template<typename R>
    std::shared_ptr<ast::stmt::Stmt<R>> while_statement() {
        const token::Token& paren {consume(token::TokenType::LeftParen, "Expected `(` after `while`")};

        std::shared_ptr<ast::expr::Expr<R>> condition {expression<R>()};

        consume(token::TokenType::RightParen, "Expected `)` after while condition");

        std::shared_ptr<ast::stmt::Stmt<R>> body {statement<R>()};

        return std::make_shared<ast::stmt::While<R>>(condition, body, paren);
    }

    template<typename R>
    std::shared_ptr<ast::stmt::Stmt<R>> for_statement() {
        const token::Token& paren {consume(token::TokenType::LeftParen, "Expected `(` after `for`")};

        std::shared_ptr<ast::stmt::Stmt<R>> initializer;

        if (match({token::TokenType::Semicolon})) {
            initializer = nullptr;
        } else if (match({token::TokenType::Let})) {
            initializer = var_declaration<R>();
        } else {
            initializer = expr_statement<R>();
        }

        std::shared_ptr<ast::expr::Expr<R>> condition;

        if (!check(token::TokenType::Semicolon)) {
            condition = expression<R>();
        }

        consume(token::TokenType::Semicolon, "Expected `;` after loop condition");

        std::shared_ptr<ast::expr::Expr<R>> post_expression;

        if (!check(token::TokenType::RightParen)) {
            post_expression = expression<R>();
        }

        consume(token::TokenType::RightParen, "Expected `)` after loop clauses");

        std::shared_ptr<ast::stmt::Stmt<R>> body {statement<R>()};

        if (post_expression != nullptr) {
            body = std::make_shared<ast::stmt::Block<R>>(
                std::vector<std::shared_ptr<ast::stmt::Stmt<R>>> {
                    body,
                    std::make_shared<ast::stmt::Expression<R>>(post_expression)
                }
            );
        }

        if (condition == nullptr) {
            condition = std::make_shared<ast::expr::Literal<R>>(object::create(true));
        }

        body = std::make_shared<ast::stmt::While<R>>(condition, body, paren);

        if (initializer != nullptr) {
            body = std::make_shared<ast::stmt::Block<R>>(
                std::vector<std::shared_ptr<ast::stmt::Stmt<R>>> {
                    initializer,
                    body
                }
            );
        }

        return body;
    }

    template<typename R>
    std::shared_ptr<ast::stmt::Stmt<R>> return_statement() {
        const token::Token& keyword {previous()};

        std::shared_ptr<ast::expr::Expr<R>> value;

        if (!check(token::TokenType::Semicolon)) {
            value = expression<R>();
        }

        consume(token::TokenType::Semicolon, "Expected `;` after return");

        return std::make_shared<ast::stmt::Return<R>>(keyword, value);
    }

    template<typename R>
    std::vector<std::shared_ptr<ast::stmt::Stmt<R>>> block() {
        std::vector<std::shared_ptr<ast::stmt::Stmt<R>>> statements;

        while (!check(token::TokenType::RightBrace) && !reached_end()) {
            statements.push_back(declaration<R>());
        }

        consume(token::TokenType::RightBrace, "Excpected `}` after block");

        return statements;
    }

    template<typename R>
    std::shared_ptr<ast::expr::Expr<R>> expression() {
        return assignment<R>();
    }

    template<typename R>
    std::shared_ptr<ast::expr::Expr<R>> assignment() {
        std::shared_ptr<ast::expr::Expr<R>> expr {logic_or<R>()};

        if (match({token::TokenType::Equal})) {
            const token::Token& equals {previous()};
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

        while (match({token::TokenType::Or})) {
            const token::Token& operator_ {previous()};
            std::shared_ptr<ast::expr::Expr<R>> right {logic_and<R>()};
            expr = std::make_shared<ast::expr::Logical<R>>(expr, operator_, right);
        }

        return expr;
    }

    template<typename R>
    std::shared_ptr<ast::expr::Expr<R>> logic_and() {
        std::shared_ptr<ast::expr::Expr<R>> expr {equality<R>()};

        while (match({token::TokenType::And})) {
            const token::Token& operator_ {previous()};
            std::shared_ptr<ast::expr::Expr<R>> right {equality<R>()};
            expr = std::make_shared<ast::expr::Logical<R>>(expr, operator_, right);
        }

        return expr;
    }

    template<typename R>
    std::shared_ptr<ast::expr::Expr<R>> equality() {
        std::shared_ptr<ast::expr::Expr<R>> expr {comparison<R>()};

        while (match({token::TokenType::BangEqual, token::TokenType::EqualEqual})) {
            const token::Token& operator_ {previous()};
            std::shared_ptr<ast::expr::Expr<R>> right {comparison<R>()};
            expr = std::make_shared<ast::expr::Binary<R>>(expr, operator_, right);
        }

        return expr;
    }

    template<typename R>
    std::shared_ptr<ast::expr::Expr<R>> comparison() {
        std::shared_ptr<ast::expr::Expr<R>> expr {term<R>()};

        while (match({token::TokenType::Greater, token::TokenType::GreaterEqual, token::TokenType::Less, token::TokenType::LessEqual})) {
            const token::Token& operator_ {previous()};
            std::shared_ptr<ast::expr::Expr<R>> right {term<R>()};
            expr = std::make_shared<ast::expr::Binary<R>>(expr, operator_, right);
        }

        return expr;
    }

    template<typename R>
    std::shared_ptr<ast::expr::Expr<R>> term() {
        std::shared_ptr<ast::expr::Expr<R>> expr {factor<R>()};

        while (match({token::TokenType::Minus, token::TokenType::Plus})) {
            const token::Token& operator_ {previous()};
            std::shared_ptr<ast::expr::Expr<R>> right {factor<R>()};
            expr = std::make_shared<ast::expr::Binary<R>>(expr, operator_, right);
        }

        return expr;
    }

    template<typename R>
    std::shared_ptr<ast::expr::Expr<R>> factor() {
        std::shared_ptr<ast::expr::Expr<R>> expr {unary<R>()};

        while (match({token::TokenType::Slash, token::TokenType::Star})) {
            const token::Token& operator_ {previous()};
            std::shared_ptr<ast::expr::Expr<R>> right {unary<R>()};
            expr = std::make_shared<ast::expr::Binary<R>>(expr, operator_, right);
        }

        return expr;
    }

    template<typename R>
    std::shared_ptr<ast::expr::Expr<R>> unary() {
        if (match({token::TokenType::Minus, token::TokenType::Bang})) {
            const token::Token& operator_ {previous()};
            std::shared_ptr<ast::expr::Expr<R>> right {unary<R>()};

            return std::make_shared<ast::expr::Unary<R>>(operator_, right);
        }

        return call<R>();
    }

    template<typename R>
    std::shared_ptr<ast::expr::Expr<R>> call() {
        std::shared_ptr<ast::expr::Expr<R>> expr {primary<R>()};

        while (true) {
            if (match({token::TokenType::LeftParen})) {
                expr = finish_call<R>(expr);
            } else if (match({token::TokenType::Dot})) {
                const token::Token& name {consume(token::TokenType::Identifier, "Expected property name after `.`")};

                expr = std::make_shared<ast::expr::Get<R>>(expr, name);
            } else {
                break;
            }
        }

        return expr;
    }

    template<typename R>
    std::shared_ptr<ast::expr::Expr<R>> primary() {
        if (match({token::TokenType::Number, token::TokenType::String})) {
            switch (previous().get_type()) {
                case token::TokenType::String:
                    return std::make_shared<ast::expr::Literal<R>>(object::create(std::get<1u>(previous().get_literal())));
                case token::TokenType::Number:
                    return std::make_shared<ast::expr::Literal<R>>(object::create(std::get<2u>(previous().get_literal())));
                default:
                    assert(false);
                    break;
            }
        }

        if (match({token::TokenType::True})) {
            return std::make_shared<ast::expr::Literal<R>>(object::create(true));
        }

        if (match({token::TokenType::False})) {
            return std::make_shared<ast::expr::Literal<R>>(object::create(false));
        }

        if (match({token::TokenType::None})) {
            return std::make_shared<ast::expr::Literal<R>>(object::create());
        }

        if (match({token::TokenType::Identifier})) {
            return std::make_shared<ast::expr::Variable<R>>(previous());
        }

        if (match({token::TokenType::LeftParen})) {
            std::shared_ptr<ast::expr::Expr<R>> expr {expression<R>()};
            consume(token::TokenType::RightParen, "Expected `)` after expression");

            return std::make_shared<ast::expr::Grouping<R>>(expr);
        }

        throw error(peek(), "Expected an expression");
    }

    template<typename R>
    std::shared_ptr<ast::expr::Expr<R>> finish_call(std::shared_ptr<ast::expr::Expr<R>> callee) {
        std::vector<std::shared_ptr<ast::expr::Expr<R>>> arguments;

        if (!check(token::TokenType::RightParen)) {
            do {
                if (arguments.size() >= 255u) {
                    error(peek(), "Too many arguments (255 maximum)");  // Don't go in panic mode
                }

                arguments.push_back(expression<R>());
            } while (match({token::TokenType::Comma}));
        }

        const token::Token& paren {consume(token::TokenType::RightParen, "Expected `)` after call arguments")};

        return std::make_shared<ast::expr::Call<R>>(callee, paren, arguments);
    }

    bool match(std::initializer_list<token::TokenType> types);
    bool check(token::TokenType type);
    const token::Token& advance();
    bool reached_end();
    const token::Token& peek();
    const token::Token& previous();
    const token::Token& consume(token::TokenType type, const std::string& message);
    ParseError error(const token::Token& token, const std::string& message);
    void synchronize();

    std::vector<token::Token> tokens;
    std::size_t current {};

    Context* ctx {nullptr};
};
