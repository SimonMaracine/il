#pragma once

#include <string>
#include <memory>
#include <vector>
#include <cstddef>
#include <functional>
#include <type_traits>

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
        Function
    };

    struct Object {
        virtual ~Object() noexcept = default;

        Type type {};
    };

    struct Callable {
        virtual ~Callable() noexcept = default;

        virtual std::shared_ptr<Object> call(Interpreter* interpreter, const std::vector<std::shared_ptr<Object>>& arguments) = 0;
        virtual std::size_t arity() const = 0;
    };

    struct None : Object {};

    struct String : Object {
        std::string value;
    };

    struct Number : Object {
        double value {};
    };

    struct Boolean : Object {
        bool value {};
    };

    struct BuiltinFunction : Object, Callable {};

    struct Function : Object, Callable {
        Function(const token::Token& name)
            : name(name) {}

        std::shared_ptr<Object> call(Interpreter* interpreter, const std::vector<std::shared_ptr<Object>>& arguments) override;
        std::size_t arity() const override;

        token::Token name;
        std::vector<token::Token> parameters;
        std::vector<std::shared_ptr<ast::stmt::Stmt<std::shared_ptr<Object>>>> body;
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

    template<typename T>
    std::shared_ptr<Object> create_builtin() {
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
