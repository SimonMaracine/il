#pragma once

#include <string>
#include <memory>
#include <initializer_list>

#include "ast.hpp"

class AstPrinter : ast::expr::Visitor<std::string>, ast::stmt::Visitor<std::string> {
public:
    std::string print(std::shared_ptr<ast::expr::Expr<std::string>> expr);
private:
    std::string parenthesize(const std::string& name, std::initializer_list<std::shared_ptr<ast::expr::Expr<std::string>>> list);
    std::string parenthesize(const std::string& name, std::initializer_list<std::string> list);

    std::string visit(ast::expr::Literal<std::string>* expr) override;
    std::string visit(ast::expr::Grouping<std::string>* expr) override;
    std::string visit(ast::expr::Unary<std::string>* expr) override;
    std::string visit(ast::expr::Binary<std::string>* expr) override;
    std::string visit(ast::expr::Variable<std::string>* expr) override;
    std::string visit(ast::expr::Assignment<std::string>* expr) override;
    std::string visit(ast::expr::Logical<std::string>* expr) override;
    std::string visit(ast::expr::Call<std::string>* expr) override;
    std::string visit(ast::expr::Get<std::string>* expr) override;
    std::string visit(ast::expr::Set<std::string>* expr) override;

    std::string visit(const ast::stmt::Expression<std::string>* stmt) override;
    std::string visit(const ast::stmt::Let<std::string>* stmt) override;
    std::string visit(const ast::stmt::Function<std::string>* stmt) override;
    std::string visit(const ast::stmt::Struct<std::string>* stmt) override;
    std::string visit(const ast::stmt::If<std::string>* stmt) override;
    std::string visit(const ast::stmt::While<std::string>* stmt) override;
    std::string visit(const ast::stmt::Block<std::string>* stmt) override;
    std::string visit(const ast::stmt::Return<std::string>* stmt) override;
};
