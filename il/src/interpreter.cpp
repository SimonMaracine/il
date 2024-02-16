#include "interpreter.hpp"

#include "token.hpp"

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
            return std::get<2u>(left) - std::get<2u>(right);
        case TokenType::Plus:
            if (left.index() == 2u && right.index() == 2u) {
                return std::get<2u>(left) + std::get<2u>(right);
            }

            if (left.index() == 1u && right.index() == 1u) {
                return std::get<1u>(left) + std::get<1u>(right);
            }

            break;
        case TokenType::Slash:
            return std::get<2u>(left) / std::get<2u>(right);
        case TokenType::Star:
            return std::get<2u>(left) * std::get<2u>(right);
        case TokenType::Greater:
            return std::get<2u>(left) > std::get<2u>(right);
        case TokenType::GreaterEqual:
            return std::get<2u>(left) >= std::get<2u>(right);
        case TokenType::Less:
            return std::get<2u>(left) < std::get<2u>(right);
        case TokenType::LessEqual:
            return std::get<2u>(left) <= std::get<2u>(right);
        case TokenType::BangEqual:
            return std::get<3u>(left) != std::get<3u>(right);
        case TokenType::EqualEqual:
            return std::get<3u>(left) == std::get<3u>(right);
        default:
            break;
    }

    return {};
}

literal::Object Interpreter::evaluate(std::shared_ptr<ast::Expr<literal::Object>> expr) {
    return expr->accept(this);
}
