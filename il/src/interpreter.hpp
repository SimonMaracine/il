#pragma once

#include <string>
#include <vector>

#include "ast.hpp"
#include "literal.hpp"
#include "token.hpp"
#include "context.hpp"
#include"environment.hpp"

class Interpreter : ast::expr::Visitor<literal::Object>, ast::stmt::Visitor<literal::Object> {
public:
    Interpreter(Context* ctx)
        : ctx(ctx) {}

    void interpret(const std::vector<std::shared_ptr<ast::stmt::Stmt<literal::Object>>>& statements);
private:
    literal::Object visit(ast::expr::Literal<literal::Object>* expr) override;
    literal::Object visit(ast::expr::Grouping<literal::Object>* expr) override;
    literal::Object visit(ast::expr::Unary<literal::Object>* expr) override;
    literal::Object visit(ast::expr::Binary<literal::Object>* expr) override;
    literal::Object visit(ast::expr::Variable<literal::Object>* expr) override;
    literal::Object visit(ast::expr::Assignment<literal::Object>* expr) override;

    literal::Object evaluate(std::shared_ptr<ast::expr::Expr<literal::Object>> expr);

    literal::Object visit(const ast::stmt::Expression<literal::Object>* stmt) override;
    literal::Object visit(const ast::stmt::Print<literal::Object>* stmt) override;
    literal::Object visit(const ast::stmt::Let<literal::Object>* stmt) override;

    void execute(std::shared_ptr<ast::stmt::Stmt<literal::Object>> statement);

    void check_number_operand(const Token& token, const literal::Object& right) const;
    void check_number_operands(const Token& token, const literal::Object& left, const literal::Object& right) const;
    void check_boolean_operand(const Token& token, const literal::Object& right) const;

    Environment environment;
    Context* ctx {nullptr};
};
