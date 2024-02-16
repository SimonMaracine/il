#pragma once

#include "ast.hpp"
#include "literal.hpp"

class Interpreter : ast::Visitor<literal::Object> {
public:

private:
    literal::Object visit(ast::Literal<literal::Object>* expr) override;
    literal::Object visit(ast::Grouping<literal::Object>* expr) override;
    literal::Object visit(ast::Unary<literal::Object>* expr) override;
    literal::Object visit(ast::Binary<literal::Object>* expr) override;

    literal::Object evaluate(std::shared_ptr<ast::Expr<literal::Object>> expr);
};
