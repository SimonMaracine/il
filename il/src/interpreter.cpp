#include "interpreter.hpp"

#include <cassert>
#include <utility>

#include "runtime_error.hpp"
#include "builtins.hpp"
#include "return.hpp"

Interpreter::Interpreter(Context* ctx)
    : current_environment(&global_environment), ctx(ctx) {
    global_environment.define("clock", object::create_builtin_function<builtins::clock>());
    global_environment.define("print", object::create_builtin_function<builtins::print>());
    global_environment.define("println", object::create_builtin_function<builtins::println>());
    global_environment.define("to_string", object::create_builtin_function<builtins::to_string>());
}

void Interpreter::interpret(const std::vector<std::shared_ptr<ast::stmt::Stmt<std::shared_ptr<object::Object>>>>& statements) {
    try {
        for (const std::shared_ptr<ast::stmt::Stmt<std::shared_ptr<object::Object>>>& statement : statements) {
            execute(statement);
        }
    } catch (const RuntimeError& e) {
        ctx->runtime_error(e.token, e.message);
    }
}

std::shared_ptr<object::Object> Interpreter::evaluate(std::shared_ptr<ast::expr::Expr<std::shared_ptr<object::Object>>> expr) {
    return expr->accept(this);
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
    return nullptr;
}

std::shared_ptr<object::Object> Interpreter::visit(ast::expr::Binary<std::shared_ptr<object::Object>>* expr) {
    std::shared_ptr<object::Object> left {evaluate(expr->left)};
    std::shared_ptr<object::Object> right {evaluate(expr->right)};

    switch (expr->operator_.get_type()) {
        case token::TokenType::Minus:
            check_number_operands(expr->operator_, left, right);

            return object::create(
                object::cast<object::Number>(left)->value - object::cast<object::Number>(right)->value
            );
        case token::TokenType::Plus:
            if (left->type == object::Type::Number && right->type == object::Type::Number) {
                return object::create(
                    object::cast<object::Number>(left)->value + object::cast<object::Number>(right)->value
                );
            }

            if (left->type == object::Type::String && right->type == object::Type::String) {
                return object::create(
                    object::cast<object::String>(left)->value + object::cast<object::String>(right)->value
                );
            }

            throw RuntimeError(expr->operator_, "Operands must be either numbers or strings");
        case token::TokenType::Slash:
            check_number_operands(expr->operator_, left, right);

            return object::create(
                object::cast<object::Number>(left)->value / object::cast<object::Number>(right)->value
            );
        case token::TokenType::Star:
            check_number_operands(expr->operator_, left, right);

            return object::create(
                object::cast<object::Number>(left)->value * object::cast<object::Number>(right)->value
            );
        case token::TokenType::Greater:
            check_number_operands(expr->operator_, left, right);

            return object::create(
                object::cast<object::Number>(left)->value > object::cast<object::Number>(right)->value
            );
        case token::TokenType::GreaterEqual:
            check_number_operands(expr->operator_, left, right);

            return object::create(
                object::cast<object::Number>(left)->value >= object::cast<object::Number>(right)->value
            );
        case token::TokenType::Less:
            check_number_operands(expr->operator_, left, right);

            return object::create(
                object::cast<object::Number>(left)->value < object::cast<object::Number>(right)->value
            );
        case token::TokenType::LessEqual:
            check_number_operands(expr->operator_, left, right);

            return object::create(
                object::cast<object::Number>(left)->value <= object::cast<object::Number>(right)->value
            );
        case token::TokenType::BangEqual:
            check_number_operands(expr->operator_, left, right);

            return object::create(
                object::cast<object::Number>(left)->value != object::cast<object::Number>(right)->value
            );
        case token::TokenType::EqualEqual:
            check_number_operands(expr->operator_, left, right);

            return object::create(
                object::cast<object::Number>(left)->value == object::cast<object::Number>(right)->value
            );
        default:
            break;
    }

    assert(false);
    return nullptr;
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

    std::shared_ptr<object::Callable> callable;

    switch (callee->type) {
        case object::Type::BuiltinFunction:
            callable = object::cast<object::BuiltinFunction>(callee);
            break;
        case object::Type::Function:
            callable = object::cast<object::Function>(callee);
            break;
        case object::Type::Struct:
            callable = object::cast<object::Struct>(callee);
            break;
        default:
            throw RuntimeError(expr->paren, "Only functions and classes are callable");
    }

    if (arguments.size() != callable->arity()) {
        const char* args_word {callable->arity() == 1u ? "argument" : "arguments"};

        throw RuntimeError(
            expr->paren,
            "Expected " + std::to_string(callable->arity()) + " " + args_word + ", but got " + std::to_string(arguments.size())
        );
    }

    return callable->call(this, arguments);
}

std::shared_ptr<object::Object> Interpreter::visit(ast::expr::Get<std::shared_ptr<object::Object>>* expr) {
    std::shared_ptr<object::Object> object {evaluate(expr->object)};

    if (object->type != object::Type::StructInstance) {
        throw RuntimeError(expr->name, "Only struct instances have properties");
    }

    return object::cast<object::StructInstance>(object)->get(expr->name);
}

std::shared_ptr<object::Object> Interpreter::visit(ast::expr::Set<std::shared_ptr<object::Object>>* expr) {
    std::shared_ptr<object::Object> object {evaluate(expr->object)};

    if (object->type != object::Type::StructInstance) {
        throw RuntimeError(expr->name, "Only struct instances have properties");
    }

    std::shared_ptr<object::Object> value {evaluate(expr->value)};

    object::cast<object::StructInstance>(object)->set(expr->name, value);

    return value;
}

void Interpreter::execute(std::shared_ptr<ast::stmt::Stmt<std::shared_ptr<object::Object>>> stmt) {
    stmt->accept(this);
}

void Interpreter::execute_block(const std::vector<std::shared_ptr<ast::stmt::Stmt<std::shared_ptr<object::Object>>>>& stmts, Environment&& environment) {
    Environment* previous_environment {current_environment};

    try {
        // Allocate a new environment and set it as the current one
        Environment block_environment {std::move(environment)};
        current_environment = &block_environment;

        for (const auto& statement : stmts) {
            execute(statement);
        }
    } catch (const RuntimeError&) {
        current_environment = previous_environment;

        // Don't handle error here
        throw;
    } catch (const Return&) {
        current_environment = previous_environment;

        // Don't handle return here
        throw;
    }

    current_environment = previous_environment;
}

std::shared_ptr<object::Object> Interpreter::visit(const ast::stmt::Expression<std::shared_ptr<object::Object>>* stmt) {
    evaluate(stmt->expression);

    return nullptr;
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

    return nullptr;
}

std::shared_ptr<object::Object> Interpreter::visit(const ast::stmt::Function<std::shared_ptr<object::Object>>* stmt) {
    std::shared_ptr<object::Object> function {object::create(stmt->name, stmt->parameters, stmt->body)};

    current_environment->define(stmt->name.get_lexeme(), function);

    return nullptr;
}

std::shared_ptr<object::Object> Interpreter::visit(const ast::stmt::Struct<std::shared_ptr<object::Object>>* stmt) {
    current_environment->define(stmt->name.get_lexeme(), nullptr);

    std::shared_ptr<object::Object> struct_ {object::create_struct(stmt->name.get_lexeme())};

    current_environment->assign(stmt->name, struct_);

    return nullptr;
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

    return nullptr;
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

    return nullptr;
}

std::shared_ptr<object::Object> Interpreter::visit(const ast::stmt::Block<std::shared_ptr<object::Object>>* stmt) {
    execute_block(stmt->statements, Environment(current_environment));

    return nullptr;
}

std::shared_ptr<object::Object> Interpreter::visit(const ast::stmt::Return<std::shared_ptr<object::Object>>* stmt) {
    std::shared_ptr<object::Object> value {
        stmt->value != nullptr
        ?
        evaluate(stmt->value)
        :
        object::create()
    };

    throw Return(value);  // Not great
}

void Interpreter::check_number_operand(const token::Token& token, const std::shared_ptr<object::Object>& right) {
    if (right->type == object::Type::Number) {
        return;
    }

    throw RuntimeError(token, "Operand must be a number");
}

void Interpreter::check_number_operands(
    const token::Token& token,
    const std::shared_ptr<object::Object>& left,
    const std::shared_ptr<object::Object>& right
) {
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
