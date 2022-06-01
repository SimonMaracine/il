#include <iostream>
#include <memory>

#include "parser.h"
#include "ast.h"

std::shared_ptr<Node> Parser::parse() {
    if (tokens.empty()) {
        return nullptr;
    }

    advance();
    std::shared_ptr<Node> result = expr();
    return result;
}

Token Parser::advance() {
    if (index < tokens.size()) {
        current_token = tokens[index];
    }

    index++;

    return current_token;
}

std::shared_ptr<Node> Parser::expr() {
    std::shared_ptr<Node> left = term();

    while (current_token.type == TokenType::Plus || current_token.type == TokenType::Minus) {
        Token token = current_token;
        advance();
        std::shared_ptr<Node> right = term();
        left = std::make_shared<BinOpNode>(token, left, right);
    }

    return left;
}

std::shared_ptr<Node> Parser::term() {
    std::shared_ptr<Node> left = factor();

    while (current_token.type == TokenType::Mul || current_token.type == TokenType::Div) {
        Token token = current_token;
        advance();
        std::shared_ptr<Node> right = factor();
        left = std::make_shared<BinOpNode>(token, left, right);
    }

    return left;
}

std::shared_ptr<Node> Parser::factor() {
    Token token = current_token;

    if (token.type == TokenType::Int || token.type == TokenType::Float) {  // TODO ?
        advance();
        return std::make_shared<NumberNode>(token);
    }
}
