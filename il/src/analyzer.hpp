#pragma once

#include <vector>
#include <memory>

#include "ast.hpp"
#include "object.hpp"
#include "context.hpp"

class Analyzer : ast::expr::Visitor<std::shared_ptr<object::Object>>, ast::stmt::Visitor<std::shared_ptr<object::Object>> {
public:
    Analyzer(Context* ctx)
        : ctx(ctx) {}

    void analyze(const std::vector<std::shared_ptr<ast::stmt::Stmt<std::shared_ptr<object::Object>>>>& statements);
private:
    void analyze(std::shared_ptr<ast::expr::Expr<std::shared_ptr<object::Object>>> expr);

    std::shared_ptr<object::Object> visit(ast::expr::Literal<std::shared_ptr<object::Object>>*) override;
    std::shared_ptr<object::Object> visit(ast::expr::Grouping<std::shared_ptr<object::Object>>* expr) override;
    std::shared_ptr<object::Object> visit(ast::expr::Unary<std::shared_ptr<object::Object>>* expr) override;
    std::shared_ptr<object::Object> visit(ast::expr::Binary<std::shared_ptr<object::Object>>* expr) override;
    std::shared_ptr<object::Object> visit(ast::expr::Variable<std::shared_ptr<object::Object>>*) override;
    std::shared_ptr<object::Object> visit(ast::expr::Assignment<std::shared_ptr<object::Object>>* expr) override;
    std::shared_ptr<object::Object> visit(ast::expr::Logical<std::shared_ptr<object::Object>>* expr) override;
    std::shared_ptr<object::Object> visit(ast::expr::Call<std::shared_ptr<object::Object>>* expr) override;
    std::shared_ptr<object::Object> visit(ast::expr::Get<std::shared_ptr<object::Object>>* expr) override;
    std::shared_ptr<object::Object> visit(ast::expr::Set<std::shared_ptr<object::Object>>* expr) override;

    void analyze(std::shared_ptr<ast::stmt::Stmt<std::shared_ptr<object::Object>>> stmt);

    std::shared_ptr<object::Object> visit(const ast::stmt::Expression<std::shared_ptr<object::Object>>* stmt) override;
    std::shared_ptr<object::Object> visit(const ast::stmt::Let<std::shared_ptr<object::Object>>* stmt) override;
    std::shared_ptr<object::Object> visit(const ast::stmt::Function<std::shared_ptr<object::Object>>* stmt) override;
    std::shared_ptr<object::Object> visit(const ast::stmt::Struct<std::shared_ptr<object::Object>>* stmt) override;
    std::shared_ptr<object::Object> visit(const ast::stmt::If<std::shared_ptr<object::Object>>* stmt) override;
    std::shared_ptr<object::Object> visit(const ast::stmt::While<std::shared_ptr<object::Object>>* stmt) override;
    std::shared_ptr<object::Object> visit(const ast::stmt::Block<std::shared_ptr<object::Object>>* stmt) override;
    std::shared_ptr<object::Object> visit(const ast::stmt::Return<std::shared_ptr<object::Object>>* stmt) override;

    bool inside_function {false};

    Context* ctx {nullptr};
};
