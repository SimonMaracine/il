#include "ast_printer.hpp"

#include <sstream>
#include <cassert>

#include "object.hpp"

std::string AstPrinter::print(std::shared_ptr<ast::expr::Expr<std::string>> expr) {
    return expr->accept(this);
}

std::string AstPrinter::parenthesize(const std::string& name, std::initializer_list<std::shared_ptr<ast::expr::Expr<std::string>>> list) {
    std::ostringstream stream;

    stream << '(' << name;

    for (const auto& expr : list) {
        stream << ' ' << expr->accept(this);
    }

    stream << ')';

    return stream.str();
}

std::string AstPrinter::parenthesize(const std::string& name, std::initializer_list<std::string> list) {
    std::ostringstream stream;

    stream << '(' << name;

    for (const auto& string : list) {
        stream << ' ' << string;
    }

    stream << ')';

    return stream.str();
}

std::string AstPrinter::visit(ast::expr::Literal<std::string>* expr) {
    return expr->value->to_string();
}

std::string AstPrinter::visit(ast::expr::Grouping<std::string>* expr) {
    return parenthesize("group", {expr->expression});
}

std::string AstPrinter::visit(ast::expr::Unary<std::string>* expr) {
    return parenthesize(expr->operator_.get_lexeme(), {expr->right});
}

std::string AstPrinter::visit(ast::expr::Binary<std::string>* expr) {
    return parenthesize(expr->operator_.get_lexeme(), {expr->left, expr->right});
}

std::string AstPrinter::visit(ast::expr::Variable<std::string>* expr) {
    return parenthesize("let", {expr->name.get_lexeme()});
}

std::string AstPrinter::visit(ast::expr::Assignment<std::string>* expr) {
    return parenthesize("=", {expr->name.get_lexeme(), expr->value->accept(this)});
}

std::string AstPrinter::visit(ast::expr::Logical<std::string>* expr) {
    return parenthesize(expr->operator_.get_lexeme(), {expr->left, expr->right});
}

std::string AstPrinter::visit(ast::expr::Call<std::string>* expr) {
    // return parenthesize("call", {expr->callee->accept(this), expr->arguments});  // FIXME
    return {};
}

std::string AstPrinter::visit(ast::expr::Get<std::string>* expr) {
    return parenthesize("get", {expr->name.get_lexeme(), expr->object->accept(this)});
}

std::string AstPrinter::visit(ast::expr::Set<std::string>* expr) {
    return parenthesize("set", {expr->name.get_lexeme(), expr->object->accept(this), expr->value->accept(this)});
}

std::string AstPrinter::visit(const ast::stmt::Expression<std::string>* stmt) {
    return {};
}

std::string AstPrinter::visit(const ast::stmt::Let<std::string>* stmt) {
    return {};
}

std::string AstPrinter::visit(const ast::stmt::Function<std::string>* stmt) {
    return {};
}

std::string AstPrinter::visit(const ast::stmt::Struct<std::string>* stmt) {
    return {};
}

std::string AstPrinter::visit(const ast::stmt::If<std::string>* stmt) {
    return {};
}

std::string AstPrinter::visit(const ast::stmt::While<std::string>* stmt) {
    return {};
}

std::string AstPrinter::visit(const ast::stmt::Block<std::string>* stmt) {
    return {};
}

std::string AstPrinter::visit(const ast::stmt::Return<std::string>* stmt) {
    return {};
}
