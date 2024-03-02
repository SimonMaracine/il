#include "interpreter.hpp"

#include <cassert>
#include <utility>
#include <unordered_map>
#include <string>
#include <cstddef>

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
            if (right->type == object::Type::Integer) {
                return object::create_integer(-object::cast<object::Integer>(right)->value);
            } else if (right->type == object::Type::Float) {
                return object::create_float(-object::cast<object::Float>(right)->value);
            }

            throw RuntimeError(expr->operator_, "Operand must be either integer or float");
        case token::TokenType::Not:
            check_boolean_operand(expr->operator_, right);

            return object::create_bool(!object::cast<object::Boolean>(right)->value);
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
            if (left->type == object::Type::Integer && right->type == object::Type::Integer) {
                return object::create_integer(
                    object::cast<object::Integer>(left)->value - object::cast<object::Integer>(right)->value
                );
            }

            if (left->type == object::Type::Float && right->type == object::Type::Float) {
                return object::create_float(
                    object::cast<object::Float>(left)->value - object::cast<object::Float>(right)->value
                );
            }

            throw RuntimeError(expr->operator_, "Operands must be either integers or floats");
        case token::TokenType::Plus:
            if (left->type == object::Type::String && right->type == object::Type::String) {
                return object::create_string(
                    object::cast<object::String>(left)->value + object::cast<object::String>(right)->value
                );
            }

            if (left->type == object::Type::Integer && right->type == object::Type::Integer) {
                return object::create_integer(
                    object::cast<object::Integer>(left)->value + object::cast<object::Integer>(right)->value
                );
            }

            if (left->type == object::Type::Float && right->type == object::Type::Float) {
                return object::create_float(
                    object::cast<object::Float>(left)->value + object::cast<object::Float>(right)->value
                );
            }

            throw RuntimeError(expr->operator_, "Operands must be either integers, floats or strings");
        case token::TokenType::Slash:
            if (left->type == object::Type::Integer && right->type == object::Type::Integer) {
                return object::create_integer(
                    object::cast<object::Integer>(left)->value / object::cast<object::Integer>(right)->value
                );
            }

            if (left->type == object::Type::Float && right->type == object::Type::Float) {
                return object::create_float(
                    object::cast<object::Float>(left)->value / object::cast<object::Float>(right)->value
                );
            }

            throw RuntimeError(expr->operator_, "Operands must be either integers or floats");
        case token::TokenType::Star:
            if (left->type == object::Type::Integer && right->type == object::Type::Integer) {
                return object::create_integer(
                    object::cast<object::Integer>(left)->value * object::cast<object::Integer>(right)->value
                );
            }

            if (left->type == object::Type::Float && right->type == object::Type::Float) {
                return object::create_float(
                    object::cast<object::Float>(left)->value * object::cast<object::Float>(right)->value
                );
            }

            throw RuntimeError(expr->operator_, "Operands must be either integers or floats");
        case token::TokenType::Greater:
            if (left->type == object::Type::Integer && right->type == object::Type::Integer) {
                return object::create_integer(
                    object::cast<object::Integer>(left)->value > object::cast<object::Integer>(right)->value
                );
            }

            if (left->type == object::Type::Float && right->type == object::Type::Float) {
                return object::create_float(
                    object::cast<object::Float>(left)->value > object::cast<object::Float>(right)->value
                );
            }

            throw RuntimeError(expr->operator_, "Operands must be either integers or floats");
        case token::TokenType::GreaterEqual:
            if (left->type == object::Type::Integer && right->type == object::Type::Integer) {
                return object::create_integer(
                    object::cast<object::Integer>(left)->value >= object::cast<object::Integer>(right)->value
                );
            }

            if (left->type == object::Type::Float && right->type == object::Type::Float) {
                return object::create_float(
                    object::cast<object::Float>(left)->value >= object::cast<object::Float>(right)->value
                );
            }

            throw RuntimeError(expr->operator_, "Operands must be either integers or floats");
        case token::TokenType::Less:
            if (left->type == object::Type::Integer && right->type == object::Type::Integer) {
                return object::create_integer(
                    object::cast<object::Integer>(left)->value < object::cast<object::Integer>(right)->value
                );
            }

            if (left->type == object::Type::Float && right->type == object::Type::Float) {
                return object::create_float(
                    object::cast<object::Float>(left)->value < object::cast<object::Float>(right)->value
                );
            }

            throw RuntimeError(expr->operator_, "Operands must be either integers or floats");
        case token::TokenType::LessEqual:
            if (left->type == object::Type::Integer && right->type == object::Type::Integer) {
                return object::create_integer(
                    object::cast<object::Integer>(left)->value <= object::cast<object::Integer>(right)->value
                );
            }

            if (left->type == object::Type::Float && right->type == object::Type::Float) {
                return object::create_float(
                    object::cast<object::Float>(left)->value <= object::cast<object::Float>(right)->value
                );
            }

            throw RuntimeError(expr->operator_, "Operands must be either integers or floats");
        case token::TokenType::BangEqual:
            if (left->type == object::Type::Integer && right->type == object::Type::Integer) {
                return object::create_integer(
                    object::cast<object::Integer>(left)->value != object::cast<object::Integer>(right)->value
                );
            }

            if (left->type == object::Type::Float && right->type == object::Type::Float) {
                return object::create_float(
                    object::cast<object::Float>(left)->value != object::cast<object::Float>(right)->value
                );
            }

            throw RuntimeError(expr->operator_, "Operands must be either integers or floats");
        case token::TokenType::EqualEqual:
            if (left->type == object::Type::Integer && right->type == object::Type::Integer) {
                return object::create_integer(
                    object::cast<object::Integer>(left)->value == object::cast<object::Integer>(right)->value
                );
            }

            if (left->type == object::Type::Float && right->type == object::Type::Float) {
                return object::create_float(
                    object::cast<object::Float>(left)->value == object::cast<object::Float>(right)->value
                );
            }

            throw RuntimeError(expr->operator_, "Operands must be either integers or floats");
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
                return object::create_bool(true);
            }

            break;
        case token::TokenType::And:
            check_boolean_value(expr->operator_, left);

            if (!object::cast<object::Boolean>(left)->value) {
                return object::create_bool(false);
            }

            break;
        default:
            break;
    }

    std::shared_ptr<object::Object> right {evaluate(expr->right)};

    check_boolean_value(expr->operator_, right);

    return object::create_bool(object::cast<object::Boolean>(right)->value);
}

std::shared_ptr<object::Object> Interpreter::visit(ast::expr::Call<std::shared_ptr<object::Object>>* expr) {
    std::shared_ptr<object::Object> callee {evaluate(expr->callee)};

    std::vector<std::shared_ptr<object::Object>> arguments;

    std::shared_ptr<object::Callable> callable;

    switch (callee->type) {
        case object::Type::BuiltinFunction:
            callable = object::cast<object::BuiltinFunction>(callee);
            break;
        case object::Type::Function:
            callable = object::cast<object::Function>(callee);
            break;
        case object::Type::Method:
            callable = object::cast<object::Method>(callee);
            break;
        case object::Type::Struct:
            callable = object::cast<object::Struct>(callee);
            break;
        default:
            throw RuntimeError(expr->paren, "Only functions and classes are callable");
    }

    if (callee->type == object::Type::Method) {
        // For methods, pass the instance first

        auto instance {object::cast<object::Method>(callee)->instance};

        assert(instance != nullptr);

        arguments.push_back(instance);
    }

    for (const std::shared_ptr<ast::expr::Expr<std::shared_ptr<object::Object>>>& argument : expr->arguments) {
        arguments.push_back(evaluate(argument));
    }

    const std::size_t arguments_size {
        callee->type == object::Type::Struct ? arguments.size() + 1u : arguments.size()
    };

    if (arguments_size != callable->arity()) {
        const char* args {callable->arity() == 1u ? "argument" : "arguments"};

        throw RuntimeError(
            expr->paren,
            "Expected " + std::to_string(callable->arity()) + " " + args + ", but got " + std::to_string(arguments_size)
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

void Interpreter::execute(const std::vector<std::shared_ptr<ast::stmt::Stmt<std::shared_ptr<object::Object>>>>& stmts, Environment&& environment) {
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
        object::create_none()
    };

    current_environment->define(stmt->name.get_lexeme(), value);

    return nullptr;
}

std::shared_ptr<object::Object> Interpreter::visit(const ast::stmt::Function<std::shared_ptr<object::Object>>* stmt) {
    std::shared_ptr<object::Object> function {object::create_function(stmt->name, stmt->parameters, stmt->body)};

    current_environment->define(stmt->name.get_lexeme(), function);

    return nullptr;
}

std::shared_ptr<object::Object> Interpreter::visit(const ast::stmt::Struct<std::shared_ptr<object::Object>>* stmt) {
    std::unordered_map<std::string, std::shared_ptr<object::Method>> methods;

    for (const auto& method : stmt->methods) {
        if (method->parameters.empty()) {
            throw RuntimeError(stmt->name, "Methods must have an instance parameter; consider adding `self`");
        }

        methods[method->name.get_lexeme()] = object::cast<object::Method>(
            object::create_method(
                method->name,
                method->parameters,
                method->body
            )
        );
    }

    current_environment->define(stmt->name.get_lexeme(), nullptr);

    std::shared_ptr<object::Object> struct_ {object::create_struct(stmt->name.get_lexeme(), methods)};

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
    execute(stmt->statements, Environment(current_environment));

    return nullptr;
}

std::shared_ptr<object::Object> Interpreter::visit(const ast::stmt::Return<std::shared_ptr<object::Object>>* stmt) {
    std::shared_ptr<object::Object> value {
        stmt->value != nullptr
        ?
        evaluate(stmt->value)
        :
        object::create_none()
    };

    throw Return(value);  // Not great
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
