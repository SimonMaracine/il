#pragma once

#include <string>
#include <memory>
#include <vector>
#include <cstddef>
#include <functional>
#include <type_traits>
#include <unordered_map>

#include "token.hpp"

class Interpreter;

namespace ast {
    namespace stmt {
        template<typename R>
        struct Stmt;
    }
}

namespace object {
    enum class Type {
        None,
        String,
        Number,
        Boolean,
        BuiltinFunction,
        Function,
        Method,
        Struct,
        StructInstance
    };

    struct Object {
        virtual ~Object() noexcept = default;

        virtual std::string to_string() const = 0;

        Type type {};
    };

    struct Callable {
        virtual ~Callable() noexcept = default;

        virtual std::shared_ptr<Object> call(Interpreter* interpreter, const std::vector<std::shared_ptr<Object>>& arguments) = 0;
        virtual std::size_t arity() const = 0;
    };

    struct None : Object {
        std::string to_string() const override;
    };

    struct String : Object {
        std::string to_string() const override;

        std::string value;
    };

    struct Number : Object {
        std::string to_string() const override;

        double value {};
    };

    struct Boolean : Object {
        std::string to_string() const override;

        bool value {};
    };

    struct BuiltinFunction : Object, Callable {
        std::string to_string() const override;
    };

    struct Function : Object, Callable {
        Function(const token::Token& name)
            : name(name) {}

        std::string to_string() const override;

        std::shared_ptr<Object> call(Interpreter* interpreter, const std::vector<std::shared_ptr<Object>>& arguments) override;
        std::size_t arity() const override;

        token::Token name;
        std::vector<token::Token> parameters;
        std::vector<std::shared_ptr<ast::stmt::Stmt<std::shared_ptr<Object>>>> body;
    };

    struct Method : Function {
        Method(const token::Token& name)
            : Function(name) {}

        std::string to_string() const override;

        std::shared_ptr<Object> instance;
    };

    struct Struct : Object, Callable, public std::enable_shared_from_this<Struct> {
        std::string to_string() const override;

        std::shared_ptr<Object> call(Interpreter* interpreter, const std::vector<std::shared_ptr<Object>>& arguments) override;
        std::size_t arity() const override;

        std::string name;
        std::unordered_map<std::string, std::shared_ptr<Method>> methods;
    };

    struct StructInstance : Object {
        std::string to_string() const override;

        std::shared_ptr<Object> get(const token::Token& name) const;
        std::shared_ptr<Object> set(const token::Token& name, std::shared_ptr<Object> value);

        std::shared_ptr<Struct> struct_;
        std::unordered_map<std::string, std::shared_ptr<Method>> methods;
        std::unordered_map<std::string, std::shared_ptr<Object>> fields;
    };

    std::shared_ptr<Object> create();
    std::shared_ptr<Object> create(const std::string& value);
    std::shared_ptr<Object> create(double value);
    std::shared_ptr<Object> create(bool value);

    std::shared_ptr<Object> create(
        const token::Token& name,
        const std::vector<token::Token>& parameters,
        const std::vector<std::shared_ptr<ast::stmt::Stmt<std::shared_ptr<Object>>>>& body
    );

    std::shared_ptr<Object> create_method(
        const token::Token& name,
        const std::vector<token::Token>& parameters,
        const std::vector<std::shared_ptr<ast::stmt::Stmt<std::shared_ptr<Object>>>>& body
    );

    std::shared_ptr<Object> create_struct(
        const std::string& name,
        const std::unordered_map<std::string, std::shared_ptr<Method>>& methods
    );

    std::shared_ptr<Object> create_struct_instance(std::shared_ptr<Struct> struct_);

    template<typename T>
    std::shared_ptr<Object> create_builtin_function() {
        static_assert(std::is_base_of_v<BuiltinFunction, T>, "T must be a builtin function derived class");

        std::shared_ptr<T> object {std::make_shared<T>()};
        object->type = Type::BuiltinFunction;

        return object;
    }

    template<typename T>
    std::shared_ptr<T> cast(const std::shared_ptr<Object>& object) {
        return std::static_pointer_cast<T>(object);
    }
}
