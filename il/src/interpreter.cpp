#include "interpreter.hpp"

#include <cassert>
#include <iostream>

#include "runtime_error.hpp"

void Interpreter::interpret(const std::vector<std::shared_ptr<ast::stmt::Stmt<literal::Object>>>& statements) {
    try {
        for (const std::shared_ptr<ast::stmt::Stmt<literal::Object>>& statement : statements) {
            execute(statement);
        }
    } catch (const RuntimeError& e) {
        ctx->runtime_error(e.token, e.message);
    }
}

literal::Object Interpreter::visit(const ast::expr::Literal<literal::Object>* expr) const {
    return expr->value;
}

literal::Object Interpreter::visit(const ast::expr::Grouping<literal::Object>* expr) const {
    return evaluate(expr->expression);
}

literal::Object Interpreter::visit(const ast::expr::Unary<literal::Object>* expr) const {
    const literal::Object right {evaluate(expr->right)};

    switch (expr->operator_.get_type()) {
        case TokenType::Minus:
            check_number_operand(expr->operator_, right);
            return -std::get<2u>(right);
        case TokenType::Bang:
            check_boolean_operand(expr->operator_, right);
            return !std::get<3u>(right);
        default:
            break;
    }

    assert(false);
    return {};
}

literal::Object Interpreter::visit(const ast::expr::Binary<literal::Object>* expr) const {
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

    assert(false);
    return {};
}

literal::Object Interpreter::visit(const ast::expr::Variable<literal::Object>* expr) const {
    return environment.get(expr->name);
}

literal::Object Interpreter::evaluate(std::shared_ptr<ast::expr::Expr<literal::Object>> expr) const {
    return expr->accept(this);
}

literal::Object Interpreter::visit(const ast::stmt::Expression<literal::Object>* stmt) {
    evaluate(stmt->expression);

    return {};
}

literal::Object Interpreter::visit(const ast::stmt::Print<literal::Object>* stmt) {
    const literal::Object value {evaluate(stmt->expression)};

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
            assert(false);
            break;
    }

    return {};
}

literal::Object Interpreter::visit(const ast::stmt::Let<literal::Object>* stmt) {
    literal::Object value;

    if (stmt->initializer != nullptr) {
        value = evaluate(stmt->initializer);
    }

    environment.define(stmt->name.get_lexeme(), value);

    return {};
}

void Interpreter::execute(std::shared_ptr<ast::stmt::Stmt<literal::Object>> statement) {
    statement->accept(this);
}

void Interpreter::check_number_operand(const Token& token, const literal::Object& right) const {
    if (right.index() == 2u) {
        return;
    }

    throw RuntimeError(token, "Operand must be a number");
}

void Interpreter::check_number_operands(const Token& token, const literal::Object& left, const literal::Object& right) const {
    if (left.index() == 2u && right.index() == 2u) {
        return;
    }

    throw RuntimeError(token, "Operands must be numbers");
}

void Interpreter::check_boolean_operand(const Token& token, const literal::Object& right) const {
    if (right.index() == 3u) {
        return;
    }

    throw RuntimeError(token, "Operand must be a boolean expression");
}
