#pragma once

#include <string>
#include <memory>
#include <vector>
#include <cstddef>
#include <functional>

class Interpreter;

namespace ast {
    namespace stmt {
        template<typename R>
        struct Function;
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
        std::shared_ptr<Object> call(Interpreter* interpreter, const std::vector<std::shared_ptr<Object>>& arguments) override;
        std::size_t arity() const override;

        std::shared_ptr<ast::stmt::Function<std::shared_ptr<Object>>> declaration;
    };

    std::shared_ptr<Object> create();
    std::shared_ptr<Object> create(const std::string& value);
    std::shared_ptr<Object> create(double value);
    std::shared_ptr<Object> create(bool value);
    std::shared_ptr<Object> create(std::shared_ptr<ast::stmt::Function<Object>> declaration);

    template<typename T>
    std::shared_ptr<Object> create_builtin() {
        std::shared_ptr<T> object {std::make_shared<T>()};
        object->type = Type::BuiltinFunction;

        return object;
    }

    template<typename T>
    std::shared_ptr<T> cast(const std::shared_ptr<Object>& object) {
        return std::static_pointer_cast<T>(object);
    }
}
