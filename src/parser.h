#pragma once

#include <vector>
#include <memory>
#include <utility>

#include "token.h"
#include "ast.h"

class Parser {
public:
    Parser(std::vector<Token>&& tokens)
        : tokens(std::move(tokens)) {}

    ~Parser() = default;

    std::shared_ptr<Node> parse();
private:
    Token advance();

    std::shared_ptr<Node> expr();
    std::shared_ptr<Node> term();
    std::shared_ptr<Node> factor();

    std::vector<Token> tokens;
    size_t index = 0;
    Token current_token;
};
