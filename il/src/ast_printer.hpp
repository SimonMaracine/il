#pragma once

#include <string>
#include <memory>
#include <sstream>

#include "ast.hpp"

class AstPrinter : Visitor<std::string> {
public:
    std::string print(std::shared_ptr<Expr<std::string>> expr) {
        return expr->accept(this);
    }

    std::string visit(Literal<std::string>* expr) override {
        // TODO

        return "";
    }

    std::string visit(Grouping<std::string>* expr) override {
        return parenthesize("group", expr->expression);
    }

    std::string visit(Unary<std::string>* expr) override {
        return parenthesize(expr->operat.get_lexeme(), expr->expression);
    }

    std::string visit(Binary<std::string>* expr) override {
        return parenthesize(expr->operat.get_lexeme(), expr->left, expr->right);
    }
private:
    std::string parenthesize(const std::string& name, std::shared_ptr<Expr<std::string>> expr) {
        std::ostringstream stream;

        stream << '(' << name;
        stream << ' ' << expr->accept(this);
        stream << ')';

        return stream.str();
    }

    std::string parenthesize(const std::string& name, std::shared_ptr<Expr<std::string>> expr1, std::shared_ptr<Expr<std::string>> expr2) {
        std::ostringstream stream;

        stream << '(' << name;
        stream << ' ' << expr1->accept(this);
        stream << ' ' << expr2->accept(this);
        stream << ')';

        return stream.str();
    }
};
