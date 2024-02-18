#pragma once

#include <string>

#include "ast.hpp"
#include "literal.hpp"
#include "token.hpp"
#include "context.hpp"

class Interpreter : ast::expr::Visitor<literal::Object> {
public:
    Interpreter(Context* ctx)
        : ctx(ctx) {}

    void interpret(std::shared_ptr<ast::expr::Expr<literal::Object>> expr);
private:
    struct RuntimeError {
        RuntimeError(const Token& token, const std::string& message)
            : token(token), message(message) {}

        Token token;
        std::string message;
    };

    literal::Object visit(ast::expr::Literal<literal::Object>* expr) override;
    literal::Object visit(ast::expr::Grouping<literal::Object>* expr) override;
    literal::Object visit(ast::expr::Unary<literal::Object>* expr) override;
    literal::Object visit(ast::expr::Binary<literal::Object>* expr) override;

    literal::Object evaluate(std::shared_ptr<ast::expr::Expr<literal::Object>> expr);

    void check_number_operand(const Token& token, const literal::Object& right);
    void check_number_operands(const Token& token, const literal::Object& left, const literal::Object& right);
    void check_boolean_operand(const Token& token, const literal::Object& right);

    Context* ctx {nullptr};
};
