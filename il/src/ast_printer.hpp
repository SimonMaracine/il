#pragma once

#include <string>
#include <memory>
#include <initializer_list>

#include "ast.hpp"

class AstPrinter : ast::expr::Visitor<std::string> {
public:
    std::string print(std::shared_ptr<ast::expr::Expr<std::string>> expr);
private:
    std::string visit(const ast::expr::Literal<std::string>* expr) const override;
    std::string visit(const ast::expr::Grouping<std::string>* expr) const override;
    std::string visit(const ast::expr::Unary<std::string>* expr) const override;
    std::string visit(const ast::expr::Binary<std::string>* expr) const override;

    std::string parenthesize(const std::string& name, std::initializer_list<std::shared_ptr<ast::expr::Expr<std::string>>> list) const;
};
