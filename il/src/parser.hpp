#pragma once

#include <vector>
#include <cstddef>
#include <memory>
#include <initializer_list>
#include <string>

#include "token.hpp"
#include "ast.hpp"

struct Context;

class Parser {
public:
    Parser(const std::vector<Token>& tokens, Context& ctx)
        : tokens(tokens), ctx(ctx) {}
private:
    using ParseError = int;

    std::shared_ptr<Expr<int>> expression();
    std::shared_ptr<Expr<int>> equality();
    std::shared_ptr<Expr<int>> comparison();
    std::shared_ptr<Expr<int>> term();
    std::shared_ptr<Expr<int>> factor();
    std::shared_ptr<Expr<int>> unary();
    std::shared_ptr<Expr<int>> primary();

    bool match(std::initializer_list<TokenType> types);
    bool check(TokenType type);
    const Token& advance();
    bool reached_end();
    const Token& peek();
    const Token& previous();
    const Token& consume(TokenType type, const std::string& message);
    ParseError error(const Token& token, const std::string& message);

    std::vector<Token> tokens;
    std::size_t current {};

    Context& ctx;
};
