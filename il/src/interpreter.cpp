#include "interpreter.hpp"

#include <cassert>
#include <iostream>
#include <utility>

#include "runtime_error.hpp"

void Interpreter::interpret(const std::vector<std::shared_ptr<ast::stmt::Stmt<std::shared_ptr<object::Object>>>>& statements) {
    try {
        for (const std::shared_ptr<ast::stmt::Stmt<std::shared_ptr<object::Object>>>& statement : statements) {
            execute(statement);
        }
    } catch (const RuntimeError& e) {
        ctx->runtime_error(e.token, e.message);
    }
}

std::shared_ptr<object::Object> Interpreter::visit(ast::expr::Literal<std::shared_ptr<object::Object>>* expr) {
    return expr->value;
}

std::shared_ptr<object::Object> Interpreter::visit(ast::expr::Grouping<std::shared_ptr<object::Object>>* expr) {
    return evaluate(expr->expression);
}

std::shared_ptr<object::Object> Interpreter::visit(ast::expr::Unary<std::shared_ptr<object::Object>>* expr) {
    std::shared_ptr<object::Object> right {evaluate(expr->right)};

    switch (expr->operator_.get_type()) {
        case token::TokenType::Minus:
            check_number_operand(expr->operator_, right);
            return object::create(-object::cast<object::Number>(right)->value);
        case token::TokenType::Bang:
            check_boolean_operand(expr->operator_, right);
            return object::create(!object::cast<object::Boolean>(right)->value);
        default:
            break;
    }

    assert(false);
    return {};
}

std::shared_ptr<object::Object> Interpreter::visit(ast::expr::Binary<std::shared_ptr<object::Object>>* expr) {
    std::shared_ptr<object::Object> left {evaluate(expr->left)};
    std::shared_ptr<object::Object> right {evaluate(expr->right)};

    switch (expr->operator_.get_type()) {
        case token::TokenType::Minus:
            check_number_operands(expr->operator_, left, right);
            return object::create(object::cast<object::Number>(left)->value - object::cast<object::Number>(right)->value);
        case token::TokenType::Plus:
            if (left->type == object::Type::Number && right->type == object::Type::Number) {
                return object::create(object::cast<object::Number>(left)->value + object::cast<object::Number>(right)->value);
            }

            if (left->type == object::Type::String && right->type == object::Type::String) {
                return object::create(object::cast<object::String>(left)->value + object::cast<object::String>(right)->value);
            }

            throw RuntimeError(expr->operator_, "Operands must be either numbers or strings");
        case token::TokenType::Slash:
            check_number_operands(expr->operator_, left, right);
            return object::create(object::cast<object::Number>(left)->value / object::cast<object::Number>(right)->value);
        case token::TokenType::Star:
            check_number_operands(expr->operator_, left, right);
            return object::create(object::cast<object::Number>(left)->value * object::cast<object::Number>(right)->value);
        case token::TokenType::Greater:
            check_number_operands(expr->operator_, left, right);
            return object::create(object::cast<object::Number>(left)->value > object::cast<object::Number>(right)->value);
        case token::TokenType::GreaterEqual:
            check_number_operands(expr->operator_, left, right);
            return object::create(object::cast<object::Number>(left)->value >= object::cast<object::Number>(right)->value);
        case token::TokenType::Less:
            check_number_operands(expr->operator_, left, right);
            return object::create(object::cast<object::Number>(left)->value < object::cast<object::Number>(right)->value);
        case token::TokenType::LessEqual:
            check_number_operands(expr->operator_, left, right);
            return object::create(object::cast<object::Number>(left)->value <= object::cast<object::Number>(right)->value);
        case token::TokenType::BangEqual:
            check_number_operands(expr->operator_, left, right);
            return object::create(object::cast<object::Number>(left)->value != object::cast<object::Number>(right)->value);
        case token::TokenType::EqualEqual:
            check_number_operands(expr->operator_, left, right);
            return object::create(object::cast<object::Number>(left)->value == object::cast<object::Number>(right)->value);
        default:
            break;
    }

    assert(false);
    return {};
}

std::shared_ptr<object::Object> Interpreter::visit(ast::expr::Variable<std::shared_ptr<object::Object>>* expr) {
    return current_environment->get(expr->name);
}

std::shared_ptr<object::Object> Interpreter::visit(ast::expr::Assignment<std::shared_ptr<object::Object>>* expr) {
    std::shared_ptr<object::Object> value {evaluate(expr->value)};

    current_environment->assign(expr->name, value);

    return value;
}

std::shared_ptr<object::Object> Interpreter::visit(ast::expr::Logical<std::shared_ptr<object::Object>>* expr) {
    std::shared_ptr<object::Object> left {evaluate(expr->left)};

    switch (expr->operator_.get_type()) {
        case token::TokenType::Or:
            check_boolean_value(expr->operator_, left);

            if (object::cast<object::Boolean>(left)->value) {
                return object::create(true);
            }

            break;
        case token::TokenType::And:
            check_boolean_value(expr->operator_, left);

            if (!object::cast<object::Boolean>(left)->value) {
                return object::create(false);
            }

            break;
        default:
            break;
    }

    std::shared_ptr<object::Object> right {evaluate(expr->right)};

    check_boolean_value(expr->operator_, right);

    return object::create(object::cast<object::Boolean>(right)->value);
}

std::shared_ptr<object::Object> Interpreter::visit(ast::expr::Call<std::shared_ptr<object::Object>>* expr) {
    std::shared_ptr<object::Object> callee {evaluate(expr->callee)};

    std::vector<std::shared_ptr<object::Object>> arguments;

    for (const std::shared_ptr<ast::expr::Expr<std::shared_ptr<object::Object>>>& argument : expr->arguments) {
        arguments.push_back(evaluate(argument));
    }

    std::shared_ptr<object::Callable> function {object::try_cast<object::Callable>(callee)};

    if (!function) {
        throw RuntimeError(expr->paren, "Only functions and classes are callable");
    }

    if (arguments.size() != function->arity()) {
        throw RuntimeError(
            expr->paren,
            "Expected `" + std::to_string(function->arity()) + "` arguments, but got `" + std::to_string(arguments.size()) + "`"
        );
    }

    return function->call(this, arguments);
}

std::shared_ptr<object::Object> Interpreter::evaluate(std::shared_ptr<ast::expr::Expr<std::shared_ptr<object::Object>>> expr) {
    return expr->accept(this);
}

std::shared_ptr<object::Object> Interpreter::visit(const ast::stmt::Expression<std::shared_ptr<object::Object>>* stmt) {
    evaluate(stmt->expression);

    return {};
}

std::shared_ptr<object::Object> Interpreter::visit(const ast::stmt::Print<std::shared_ptr<object::Object>>* stmt) {
    std::shared_ptr<object::Object> value {evaluate(stmt->expression)};

    switch (value->type) {
        case object::Type::None:
            std::cout << "none" << '\n';
            break;
        case object::Type::String:
            std::cout << object::cast<object::String>(value)->value << '\n';
            break;
        case object::Type::Number:
            std::cout << object::cast<object::Number>(value)->value << '\n';
            break;
        case object::Type::Boolean:
            std::cout << std::boolalpha << object::cast<object::Boolean>(value)->value << '\n';
            break;
        default:
            assert(false);
            break;
    }

    return {};
}

std::shared_ptr<object::Object> Interpreter::visit(const ast::stmt::Let<std::shared_ptr<object::Object>>* stmt) {
    std::shared_ptr<object::Object> value {
        stmt->initializer != nullptr
        ?
        evaluate(stmt->initializer)
        :
        object::create()
    };

    current_environment->define(stmt->name.get_lexeme(), value);

    return {};
}

std::shared_ptr<object::Object> Interpreter::visit(const ast::stmt::If<std::shared_ptr<object::Object>>* stmt) {
    std::shared_ptr<object::Object> value {evaluate(stmt->condition)};

    check_boolean_value(stmt->paren, value);

    if (object::cast<object::Boolean>(value)->value) {
        execute(stmt->then_branch);
    } else {
        if (stmt->else_branch != nullptr) {
            execute(stmt->else_branch);
        }
    }

    return {};
}

std::shared_ptr<object::Object> Interpreter::visit(const ast::stmt::While<std::shared_ptr<object::Object>>* stmt) {
    while (true) {
        std::shared_ptr<object::Object> value {evaluate(stmt->condition)};

        check_boolean_value(stmt->paren, value);

        if (!object::cast<object::Boolean>(value)->value) {
            break;
        }

        execute(stmt->body);
    }

    return {};
}

std::shared_ptr<object::Object> Interpreter::visit(const ast::stmt::Block<std::shared_ptr<object::Object>>* stmt) {
    execute_block(stmt->statements, Environment(current_environment));

    return {};
}

void Interpreter::execute(std::shared_ptr<ast::stmt::Stmt<std::shared_ptr<object::Object>>> statement) {
    statement->accept(this);
}

void Interpreter::execute_block(const std::vector<std::shared_ptr<ast::stmt::Stmt<std::shared_ptr<object::Object>>>>& statements, Environment&& environment) {
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

void Interpreter::check_number_operand(const token::Token& token, const std::shared_ptr<object::Object>& right) {
    if (right->type == object::Type::Number) {
        return;
    }

    throw RuntimeError(token, "Operand must be a number");
}

void Interpreter::check_number_operands(const token::Token& token, const std::shared_ptr<object::Object>& left, const std::shared_ptr<object::Object>& right) {
    if (left->type == object::Type::Number && right->type == object::Type::Number) {
        return;
    }

    throw RuntimeError(token, "Operands must be numbers");
}

void Interpreter::check_boolean_operand(const token::Token& token, const std::shared_ptr<object::Object>& right) {
    if (right->type == object::Type::Boolean) {
        return;
    }

    throw RuntimeError(token, "Operand must be a boolean expression");
}

void Interpreter::check_boolean_value(const token::Token& token, const std::shared_ptr<object::Object>& value) {
    if (value->type == object::Type::Boolean) {
        return;
    }

    throw RuntimeError(token, "Value must be a boolean expression");
}
