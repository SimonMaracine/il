#pragma once

#include <string>
#include <memory>
#include <initializer_list>

#include "ast.hpp"

class AstPrinter : ast::Visitor<std::string> {
public:
    std::string print(std::shared_ptr<ast::Expr<std::string>> expr);
private:
    std::string visit(ast::Literal<std::string>* expr) override;
    std::string visit(ast::Grouping<std::string>* expr) override;
    std::string visit(ast::Unary<std::string>* expr) override;
    std::string visit(ast::Binary<std::string>* expr) override;

    std::string parenthesize(const std::string& name, std::initializer_list<std::shared_ptr<ast::Expr<std::string>>> list);
};
