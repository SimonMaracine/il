#include "object.hpp"

#include "ast.hpp"
#include "interpreter.hpp"
#include "environment.hpp"

namespace object {
    std::shared_ptr<Object> Function::call(Interpreter* interpreter, const std::vector<std::shared_ptr<Object>>& arguments) {
        Environment environment {&interpreter->global_environment};

        // Create all the local variables
        for (std::size_t i {0u}; i < declaration->parameters.size(); i++) {
            environment.define(declaration->parameters[i].get_lexeme(), arguments[i]);
        }

        interpreter->execute_block(declaration->body, std::move(environment));

        return create();
    }

    std::shared_ptr<Object> create() {
        std::shared_ptr<None> object {std::make_shared<None>()};
        object->type = Type::None;

        return object;
    }

    std::shared_ptr<Object> create(const std::string& value) {
        std::shared_ptr<String> object {std::make_shared<String>()};
        object->type = Type::String;
        object->value = value;

        return object;
    }

    std::shared_ptr<Object> create(double value) {
        std::shared_ptr<Number> object {std::make_shared<Number>()};
        object->type = Type::Number;
        object->value = value;

        return object;
    }

    std::shared_ptr<Object> create(bool value) {
        std::shared_ptr<Boolean> object {std::make_shared<Boolean>()};
        object->type = Type::Boolean;
        object->value = value;

        return object;
    }

    std::shared_ptr<Object> create(const BuiltinFunction::Call& call, std::size_t arity) {
        std::shared_ptr<BuiltinFunction> object {std::make_shared<BuiltinFunction>()};
        object->type = Type::BuiltinFunction;
        object->call = call;
        object->arity = arity;

        return object;
    }

    std::shared_ptr<Object> create(std::shared_ptr<ast::stmt::Function<std::shared_ptr<Object>>> declaration, std::size_t arity) {
        std::shared_ptr<Function> object {std::make_shared<Function>()};
        object->type = Type::Function;
        object->declaration = declaration;
        object->arity = arity;

        return object;
    }
}
