#include "object.hpp"

#include <utility>
#include <cassert>

#include "ast.hpp"
#include "interpreter.hpp"
#include "environment.hpp"
#include "return.hpp"
#include "runtime_error.hpp"

namespace object {
    std::string None::to_string() const {
        return "none";
    }

    std::string String::to_string() const {
        return value;
    }

    std::string Integer::to_string() const {
        return std::to_string(value);
    }

    std::string Float::to_string() const {
        return std::to_string(value);
    }

    std::string Boolean::to_string() const {
        return value ? "true" : "false";
    }

    std::string BuiltinFunction::to_string() const {
        return "<builtin function>";
    }

    std::string Function::to_string() const {
        return "<function " + name.get_lexeme() + ">";
    }

    std::string Method::to_string() const {
        return "<method " + name.get_lexeme() + ">";
    }

    std::string Struct::to_string() const {
        return "<struct " + name + ">";
    }

    std::string StructInstance::to_string() const {
        return "<" + struct_->name + " instance>";
    }

    std::shared_ptr<Object> StructInstance::get(const token::Token& name) const {
        if (fields.find(name.get_lexeme()) != fields.cend()) {
            return fields.at(name.get_lexeme());
        }

        if (methods.find(name.get_lexeme()) != methods.cend()) {
            return methods.at(name.get_lexeme());
        }

        throw RuntimeError(name, "Undefined attribute `" + name.get_lexeme() + "`");
    }

    std::shared_ptr<Object> StructInstance::set(const token::Token& name, std::shared_ptr<Object> value) {
        return fields[name.get_lexeme()] = value;
    }

    std::shared_ptr<Object> Function::call(
        Interpreter* interpreter,
        const std::vector<std::shared_ptr<Object>>& arguments,
        const token::Token&
    ) {
        Environment environment {&interpreter->global_environment};

        // Create all the local variables
        for (std::size_t i {0u}; i < parameters.size(); i++) {
            environment.define(parameters[i].get_lexeme(), arguments[i]);
        }

        // Using exceptions for control flow, not great
        try {
            interpreter->execute(body, std::move(environment));
        } catch (const Return& return_value) {
            return return_value.value;
        }

        return create_none();
    }

    std::size_t Function::arity() const {
        return parameters.size();
    }

    std::shared_ptr<Object> Struct::call(
        Interpreter* interpreter,
        const std::vector<std::shared_ptr<Object>>& arguments,
        const token::Token& token
    ) {
        std::shared_ptr<StructInstance> instance {cast<StructInstance>(create_struct_instance(shared_from_this()))};

        // Bind the instance to the methods
        for (auto& [_, method] : instance->methods) {
            method->instance = instance;
        }

        // Call the initialzer, if there is one
        // Instance argument must be passed here

        auto arguments_and_self {arguments};
        arguments_and_self.insert(arguments_and_self.cbegin(), instance);

        if (methods.find("init") != methods.cend()) {
            methods.at("init")->call(interpreter, arguments_and_self, token);
        }

        return instance;
    }

    std::size_t Struct::arity() const {
        if (methods.find("init") != methods.cend()) {
            return methods.at("init")->arity();
        }

        return 1u;
    }

    std::shared_ptr<Object> create_none() {
        std::shared_ptr<None> object {std::make_shared<None>()};
        object->type = Type::None;

        return object;
    }

    std::shared_ptr<Object> create_string(const std::string& value) {
        std::shared_ptr<String> object {std::make_shared<String>()};
        object->type = Type::String;
        object->value = value;

        return object;
    }

    std::shared_ptr<Object> create_integer(long long value) {
        std::shared_ptr<Integer> object {std::make_shared<Integer>()};
        object->type = Type::Integer;
        object->value = value;

        return object;
    }

    std::shared_ptr<Object> create_float(double value) {
        std::shared_ptr<Float> object {std::make_shared<Float>()};
        object->type = Type::Float;
        object->value = value;

        return object;
    }

    std::shared_ptr<Object> create_bool(bool value) {
        std::shared_ptr<Boolean> object {std::make_shared<Boolean>()};
        object->type = Type::Boolean;
        object->value = value;

        return object;
    }

    std::shared_ptr<Object> create_function(
        const token::Token& name,
        const std::vector<token::Token>& parameters,
        const std::vector<std::shared_ptr<ast::stmt::Stmt<std::shared_ptr<Object>>>>& body
    ) {
        std::shared_ptr<Function> object {std::make_shared<Function>(name)};
        object->type = Type::Function;
        object->parameters = parameters;
        object->body = body;

        return object;
    }

    std::shared_ptr<Object> create_struct(
        const std::string& name,
        const std::unordered_map<std::string, std::shared_ptr<Method>>& methods
    ) {
        std::shared_ptr<Struct> object {std::make_shared<Struct>()};
        object->type = Type::Struct;
        object->name = name;
        object->methods = methods;

        return object;
    }

    std::shared_ptr<Object> create_struct_instance(std::shared_ptr<Struct> struct_) {
        std::shared_ptr<StructInstance> object {std::make_shared<StructInstance>()};
        object->type = Type::StructInstance;
        object->struct_ = struct_;

        // Create deep copies of the methods
        for (const auto& [name, method] : struct_->methods) {
            object->methods[name] = std::make_shared<Method>(*method);
        }

        return object;
    }

    std::shared_ptr<Object> create_method(
        const token::Token& name,
        const std::vector<token::Token>& parameters,
        const std::vector<std::shared_ptr<ast::stmt::Stmt<std::shared_ptr<Object>>>>& body
    ) {
        std::shared_ptr<Method> object {std::make_shared<Method>(name)};
        object->type = Type::Method;
        object->parameters = parameters;
        object->body = body;

        assert(parameters.size() > 0u);

        return object;
    }
}
