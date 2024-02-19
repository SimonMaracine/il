#pragma once

#include <string>
#include <vector>

#include "ast.hpp"
#include "literal.hpp"
#include "token.hpp"
#include "context.hpp"

class Interpreter : ast::expr::Visitor<literal::Object>, ast::stmt::Visitor<literal::Object> {
public:
    Interpreter(Context* ctx)
        : ctx(ctx) {}

    void interpret(const std::vector<std::shared_ptr<ast::stmt::Stmt<literal::Object>>>& statements);
private:
    struct RuntimeError {
        RuntimeError(const Token& token, const std::string& message)
            : token(token), message(message) {}

        Token token;
        std::string message;
    };

    literal::Object visit(const ast::expr::Literal<literal::Object>* expr) const override;
    literal::Object visit(const ast::expr::Grouping<literal::Object>* expr) const override;
    literal::Object visit(const ast::expr::Unary<literal::Object>* expr) const override;
    literal::Object visit(const ast::expr::Binary<literal::Object>* expr) const override;

    literal::Object evaluate(std::shared_ptr<ast::expr::Expr<literal::Object>> expr) const;

    literal::Object visit(const ast::stmt::Expression<literal::Object>* stmt) const override;
    literal::Object visit(const ast::stmt::Print<literal::Object>* stmt) const override;

    void execute(std::shared_ptr<ast::stmt::Stmt<literal::Object>> statement);

    void check_number_operand(const Token& token, const literal::Object& right) const;
    void check_number_operands(const Token& token, const literal::Object& left, const literal::Object& right) const;
    void check_boolean_operand(const Token& token, const literal::Object& right) const;

    Context* ctx {nullptr};
};
