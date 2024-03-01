#include "analyzer.hpp"

void Analyzer::analyze(const std::vector<std::shared_ptr<ast::stmt::Stmt<std::shared_ptr<object::Object>>>>& statements) {
    for (const auto& statement : statements) {
        analyze(statement);
    }
}

void Analyzer::analyze(std::shared_ptr<ast::expr::Expr<std::shared_ptr<object::Object>>> expr) {
    expr->accept(this);
}

std::shared_ptr<object::Object> Analyzer::visit(ast::expr::Literal<std::shared_ptr<object::Object>>*) {
    return nullptr;
}

std::shared_ptr<object::Object> Analyzer::visit(ast::expr::Grouping<std::shared_ptr<object::Object>>* expr) {
    analyze(expr->expression);

    return nullptr;
}

std::shared_ptr<object::Object> Analyzer::visit(ast::expr::Unary<std::shared_ptr<object::Object>>* expr) {
    analyze(expr->right);

    return nullptr;
}

std::shared_ptr<object::Object> Analyzer::visit(ast::expr::Binary<std::shared_ptr<object::Object>>* expr) {
    analyze(expr->left);
    analyze(expr->right);

    return nullptr;
}

std::shared_ptr<object::Object> Analyzer::visit(ast::expr::Variable<std::shared_ptr<object::Object>>*) {
    return nullptr;
}

std::shared_ptr<object::Object> Analyzer::visit(ast::expr::Assignment<std::shared_ptr<object::Object>>* expr) {
    analyze(expr->value);

    return nullptr;
}

std::shared_ptr<object::Object> Analyzer::visit(ast::expr::Logical<std::shared_ptr<object::Object>>* expr) {
    analyze(expr->left);
    analyze(expr->right);

    return nullptr;
}

std::shared_ptr<object::Object> Analyzer::visit(ast::expr::Call<std::shared_ptr<object::Object>>* expr) {
    analyze(expr->callee);

    for (const auto& argument : expr->arguments) {
        analyze(argument);
    }

    return nullptr;
}

std::shared_ptr<object::Object> Analyzer::visit(ast::expr::Get<std::shared_ptr<object::Object>>* expr) {
    analyze(expr->object);

    return nullptr;
}

std::shared_ptr<object::Object> Analyzer::visit(ast::expr::Set<std::shared_ptr<object::Object>>* expr) {
    analyze(expr->object);
    analyze(expr->value);

    return nullptr;
}

void Analyzer::analyze(std::shared_ptr<ast::stmt::Stmt<std::shared_ptr<object::Object>>> stmt) {
    stmt->accept(this);
}

std::shared_ptr<object::Object> Analyzer::visit(const ast::stmt::Expression<std::shared_ptr<object::Object>>* stmt) {
    analyze(stmt->expression);

    return nullptr;
}

std::shared_ptr<object::Object> Analyzer::visit(const ast::stmt::Let<std::shared_ptr<object::Object>>* stmt) {
    if (stmt->initializer != nullptr) {
        analyze(stmt->initializer);
    }

    return nullptr;
}

std::shared_ptr<object::Object> Analyzer::visit(const ast::stmt::Function<std::shared_ptr<object::Object>>* stmt) {
    if (inside_function) {
        ctx->error(stmt->name, "Functions can only be declared at the top level");
    }

    inside_function = true;

    analyze(stmt->body);

    inside_function = false;

    return nullptr;
}

std::shared_ptr<object::Object> Analyzer::visit(const ast::stmt::Struct<std::shared_ptr<object::Object>>* stmt) {
    if (inside_function) {
        ctx->error(stmt->name, "Structs can only be declared at the top level");
    }

    for (const auto& method : stmt->methods) {
        analyze(method);
    }

    return nullptr;
}

std::shared_ptr<object::Object> Analyzer::visit(const ast::stmt::If<std::shared_ptr<object::Object>>* stmt) {
    analyze(stmt->condition);
    analyze(stmt->then_branch);

    if (stmt->else_branch != nullptr) {
        analyze(stmt->else_branch);
    }

    return nullptr;
}

std::shared_ptr<object::Object> Analyzer::visit(const ast::stmt::While<std::shared_ptr<object::Object>>* stmt) {
    analyze(stmt->condition);
    analyze(stmt->body);

    return nullptr;
}

std::shared_ptr<object::Object> Analyzer::visit(const ast::stmt::Block<std::shared_ptr<object::Object>>* stmt) {
    analyze(stmt->statements);

    return nullptr;
}

std::shared_ptr<object::Object> Analyzer::visit(const ast::stmt::Return<std::shared_ptr<object::Object>>* stmt) {
    if (!inside_function) {
        ctx->error(stmt->keyword, "Can only return from inside a function");
    }

    if (stmt->value != nullptr) {
        analyze(stmt->value);
    }

    return nullptr;
}
