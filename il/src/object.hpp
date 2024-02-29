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

    struct BuiltinFunction : Object {
        using Call = std::function<std::shared_ptr<Object>(Interpreter*, const std::vector<std::shared_ptr<Object>>&)>;

        Call call;
        std::size_t arity {};
    };

    struct Function : Object {
        std::shared_ptr<Object> call(Interpreter* interpreter, const std::vector<std::shared_ptr<Object>>& arguments);

        std::shared_ptr<ast::stmt::Function<std::shared_ptr<Object>>> declaration;
        std::size_t arity {};
    };

    std::shared_ptr<Object> create();
    std::shared_ptr<Object> create(const std::string& value);
    std::shared_ptr<Object> create(double value);
    std::shared_ptr<Object> create(bool value);
    std::shared_ptr<Object> create(const BuiltinFunction::Call& call, std::size_t arity);
    std::shared_ptr<Object> create(std::shared_ptr<ast::stmt::Function<Object>> declaration);

    template<typename T>
    std::shared_ptr<T> cast(const std::shared_ptr<Object>& object) {
        return std::static_pointer_cast<T>(object);
    }
}
