#include "interpreter.hpp"

#include <cassert>
#include <iostream>
#include <utility>

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

literal::Object Interpreter::visit(ast::expr::Literal<literal::Object>* expr) {
    return expr->value;
}

literal::Object Interpreter::visit(ast::expr::Grouping<literal::Object>* expr) {
    return evaluate(expr->expression);
}

literal::Object Interpreter::visit(ast::expr::Unary<literal::Object>* expr) {
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

literal::Object Interpreter::visit(ast::expr::Binary<literal::Object>* expr) {
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

literal::Object Interpreter::visit(ast::expr::Variable<literal::Object>* expr) {
    return current_environment->get(expr->name);
}

literal::Object Interpreter::visit(ast::expr::Assignment<literal::Object>* expr) {
    const literal::Object value {evaluate(expr->value)};

    current_environment->assign(expr->name, value);

    return value;
}

literal::Object Interpreter::visit(ast::expr::Logical<literal::Object>* expr) {
    const literal::Object left {evaluate(expr->left)};

    switch (expr->operator_.get_type()) {
        case TokenType::Or:
            // check_boolean_value(, value);  // FIXME

            if (std::get<3u>(left)) {
                return true;
            }

            break;
        case TokenType::And:
            // check_boolean_value(, value);  // FIXME

            if (!std::get<3u>(left)) {
                return false;
            }

            break;
        default:
            break;
    }

    const literal::Object right {evaluate(expr->right)};

    // check_boolean_value(, value);  // FIXME

    return std::get<3u>(right);
}

literal::Object Interpreter::evaluate(std::shared_ptr<ast::expr::Expr<literal::Object>> expr) {
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

    current_environment->define(stmt->name.get_lexeme(), value);

    return {};
}

literal::Object Interpreter::visit(const ast::stmt::If<literal::Object>* stmt) {
    const literal::Object value {evaluate(stmt->condition)};

    // check_boolean_value(, value);  // FIXME

    if (std::get<3u>(value)) {
        execute(stmt->then_branch);
    } else {
        if (stmt->else_branch != nullptr) {
            execute(stmt->else_branch);
        }
    }

    return {};
}

literal::Object Interpreter::visit(const ast::stmt::While<literal::Object>* stmt) {
    while (true) {
        const literal::Object value {evaluate(stmt->condition)};

        // check_boolean_value(, value);  // FIXME

        if (!std::get<3u>(value)) {
            break;
        }

        execute(stmt->body);
    }

    return {};
}

literal::Object Interpreter::visit(const ast::stmt::Block<literal::Object>* stmt) {
    execute_block(stmt->statements, Environment(current_environment));

    return {};
}

void Interpreter::execute(std::shared_ptr<ast::stmt::Stmt<literal::Object>> statement) {
    statement->accept(this);
}

void Interpreter::execute_block(const std::vector<std::shared_ptr<ast::stmt::Stmt<literal::Object>>>& statements, Environment&& environment) {
    Environment* previous {current_environment};

    try {
        // Allocate a new environment and set it as the current one
        Environment block_environment {std::move(environment)};
        current_environment = &block_environment;

        for (const auto& statement : statements) {
            execute(statement);
        }
    } catch (const RuntimeError&) {
        // Reset the previous environment
        current_environment = previous;

        // Don't handle this exception here
        throw;
    }

    // Reset the previous environment
    current_environment = previous;
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

void Interpreter::check_boolean_operand(const Token& token, const literal::Object& right) {
    if (right.index() == 3u) {
        return;
    }

    throw RuntimeError(token, "Operand must be a boolean expression");
}

void Interpreter::check_boolean_value(const Token& token, const literal::Object& value) {
    if (value.index() == 3u) {
        return;
    }

    throw RuntimeError(token, "Value must be a boolean expression");
}
