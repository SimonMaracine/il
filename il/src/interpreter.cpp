#include "interpreter.hpp"

#include <iostream>  // FIXME delete

void Interpreter::interpret(std::shared_ptr<ast::Expr<literal::Object>> expr) {
    try {
        const literal::Object value {evaluate(expr)};

        switch (value.index()) {
            case 0u:
                std::cout << "null" << '\n';
                break;
            case 1u:
                std::cout << std::get<1u>(value) << '\n';
                break;
            case 2u:
                std::cout << std::get<2u>(value) << '\n';
                break;
            case 3u:
                std::cout << (std::get<3u>(value) ? "true" : "false") << '\n';
                break;
            default:
                break;
        }
    } catch (const RuntimeError& e) {
        ctx->runtime_error(e.token, e.message);
    }
}

literal::Object Interpreter::visit(ast::Literal<literal::Object>* expr) {
    return expr->value;
}

literal::Object Interpreter::visit(ast::Grouping<literal::Object>* expr) {
    return evaluate(expr->expression);
}

literal::Object Interpreter::visit(ast::Unary<literal::Object>* expr) {
    const literal::Object right {evaluate(expr->right)};

    switch (expr->operator_.get_type()) {
        case TokenType::Minus:
            check_number_operand(expr->operator_, right);
            return -std::get<2u>(right);
        case TokenType::Bang:
            return !std::get<3u>(right);
        default:
            break;
    }

    return {};
}

literal::Object Interpreter::visit(ast::Binary<literal::Object>* expr) {
    const literal::Object left {evaluate(expr->left)};
    const literal::Object right {evaluate(expr->right)};

    switch (expr->operator_.get_type()) {
        case TokenType::Minus:
            check_number_operands(expr->operator_, left, right);
            return std::get<2u>(left) - std::get<2u>(right);
        case TokenType::Plus:
            if (left.index() == 2u && right.index() == 2u) {
                return std::get<2u>(left) + std::get<2u>(right);
            }

            if (left.index() == 1u && right.index() == 1u) {
                return std::get<1u>(left) + std::get<1u>(right);
            }

            throw RuntimeError(expr->operator_, "Operands must be either numbers or strings");
        case TokenType::Slash:
            check_number_operands(expr->operator_, left, right);
            return std::get<2u>(left) / std::get<2u>(right);
        case TokenType::Star:
            check_number_operands(expr->operator_, left, right);
            return std::get<2u>(left) * std::get<2u>(right);
        case TokenType::Greater:
            check_number_operands(expr->operator_, left, right);
            return std::get<2u>(left) > std::get<2u>(right);
        case TokenType::GreaterEqual:
            check_number_operands(expr->operator_, left, right);
            return std::get<2u>(left) >= std::get<2u>(right);
        case TokenType::Less:
            check_number_operands(expr->operator_, left, right);
            return std::get<2u>(left) < std::get<2u>(right);
        case TokenType::LessEqual:
            check_number_operands(expr->operator_, left, right);
            return std::get<2u>(left) <= std::get<2u>(right);
        case TokenType::BangEqual:
            check_number_operands(expr->operator_, left, right);
            return std::get<2u>(left) != std::get<2u>(right);
        case TokenType::EqualEqual:
            check_number_operands(expr->operator_, left, right);
            return std::get<2u>(left) == std::get<2u>(right);
        default:
            break;
    }

    return {};
}

literal::Object Interpreter::evaluate(std::shared_ptr<ast::Expr<literal::Object>> expr) {
    return expr->accept(this);
}

void Interpreter::check_number_operand(const Token& token, const literal::Object& right) {
    if (right.index() == 2u) {
        return;
    }

    throw RuntimeError(token, "Operand must be a number");
}

void Interpreter::check_number_operands(const Token& token, const literal::Object& left, const literal::Object& right) {
    if (left.index() == 2u && right.index() == 2u) {
        return;
    }

    throw RuntimeError(token, "Operands must be numbers");
}
