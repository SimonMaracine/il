#pragma once

#include <iostream>
#include <memory>
#include <string>

#include "token.h"

enum class NodeType {
    None,
    Number,
    BinOp
};

struct Node {
    Node() = default;
    virtual ~Node() = default;

    NodeType type = NodeType::None;
};

struct NumberNode : public Node {
    NumberNode() { type = NodeType::Number; };
    NumberNode(const Token& number_token)
        : number_token(number_token) { type = NodeType::Number; }
    virtual ~NumberNode() = default;

    Token number_token;
};

struct BinOpNode : public Node {
    BinOpNode() { type = NodeType::BinOp; };
    BinOpNode(const Token& bin_op_token, std::shared_ptr<Node> left, std::shared_ptr<Node> right)
        : bin_op_token(bin_op_token), left(left), right(right) { type = NodeType::BinOp; }
    virtual ~BinOpNode() = default;

    Token bin_op_token;
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;
};

std::ostream& operator<<(std::ostream& stream, const Node& node);
