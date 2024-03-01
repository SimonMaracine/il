#pragma once

#include <vector>
#include <memory>

#include "ast.hpp"
#include "object.hpp"
#include "token.hpp"
#include "context.hpp"
#include "environment.hpp"

class Interpreter : ast::expr::Visitor<std::shared_ptr<object::Object>>, ast::stmt::Visitor<std::shared_ptr<object::Object>> {
public:
    Interpreter(Context* ctx);

    void interpret(const std::vector<std::shared_ptr<ast::stmt::Stmt<std::shared_ptr<object::Object>>>>& statements);
private:
    std::shared_ptr<object::Object> evaluate(std::shared_ptr<ast::expr::Expr<std::shared_ptr<object::Object>>> expr);

    std::shared_ptr<object::Object> visit(ast::expr::Literal<std::shared_ptr<object::Object>>* expr) override;
    std::shared_ptr<object::Object> visit(ast::expr::Grouping<std::shared_ptr<object::Object>>* expr) override;
    std::shared_ptr<object::Object> visit(ast::expr::Unary<std::shared_ptr<object::Object>>* expr) override;
    std::shared_ptr<object::Object> visit(ast::expr::Binary<std::shared_ptr<object::Object>>* expr) override;
    std::shared_ptr<object::Object> visit(ast::expr::Variable<std::shared_ptr<object::Object>>* expr) override;
    std::shared_ptr<object::Object> visit(ast::expr::Assignment<std::shared_ptr<object::Object>>* expr) override;
    std::shared_ptr<object::Object> visit(ast::expr::Logical<std::shared_ptr<object::Object>>* expr) override;
    std::shared_ptr<object::Object> visit(ast::expr::Call<std::shared_ptr<object::Object>>* expr) override;
    std::shared_ptr<object::Object> visit(ast::expr::Get<std::shared_ptr<object::Object>>* expr) override;
    std::shared_ptr<object::Object> visit(ast::expr::Set<std::shared_ptr<object::Object>>* expr) override;

    void execute(std::shared_ptr<ast::stmt::Stmt<std::shared_ptr<object::Object>>> stmt);
    void execute_block(const std::vector<std::shared_ptr<ast::stmt::Stmt<std::shared_ptr<object::Object>>>>& stmts, Environment&& environment);

    std::shared_ptr<object::Object> visit(const ast::stmt::Expression<std::shared_ptr<object::Object>>* stmt) override;
    std::shared_ptr<object::Object> visit(const ast::stmt::Let<std::shared_ptr<object::Object>>* stmt) override;
    std::shared_ptr<object::Object> visit(const ast::stmt::Function<std::shared_ptr<object::Object>>* stmt) override;
    std::shared_ptr<object::Object> visit(const ast::stmt::Struct<std::shared_ptr<object::Object>>* stmt) override;
    std::shared_ptr<object::Object> visit(const ast::stmt::If<std::shared_ptr<object::Object>>* stmt) override;
    std::shared_ptr<object::Object> visit(const ast::stmt::While<std::shared_ptr<object::Object>>* stmt) override;
    std::shared_ptr<object::Object> visit(const ast::stmt::Block<std::shared_ptr<object::Object>>* stmt) override;
    std::shared_ptr<object::Object> visit(const ast::stmt::Return<std::shared_ptr<object::Object>>* stmt) override;

    static void check_number_operand(const token::Token& token, const std::shared_ptr<object::Object>& right);
    static void check_number_operands(
        const token::Token& token,
        const std::shared_ptr<object::Object>& left,
        const std::shared_ptr<object::Object>& right
    );
    static void check_boolean_operand(const token::Token& token, const std::shared_ptr<object::Object>& right);
    static void check_boolean_value(const token::Token& token, const std::shared_ptr<object::Object>& value);

    Environment global_environment;
    Environment* current_environment {nullptr};
    Context* ctx {nullptr};

    friend struct object::Function;
};
