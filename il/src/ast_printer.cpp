#include "ast_printer.hpp"

#include <sstream>
#include <cassert>

std::string AstPrinter::print(std::shared_ptr<ast::expr::Expr<std::string>> expr) {
    return expr->accept(this);
}

std::string AstPrinter::visit(ast::expr::Literal<std::string>* expr) {
    switch (expr->value.index()) {
        case 0u:
            return "null";
        case 1u:
            return std::get<1u>(expr->value);
        case 2u:
            return std::to_string(std::get<2u>(expr->value));
        case 3u:
            return std::to_string(std::get<3u>(expr->value));
        default:
            assert(false);
            break;
    }

    return {};
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
    return {};
}

std::string AstPrinter::visit(ast::expr::Assignment<std::string>* expr) {
    return {};
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
