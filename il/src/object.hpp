#pragma once

#include <string>
#include <memory>
#include <vector>
#include <cstddef>
#include <functional>

class Interpreter;

namespace object {
    enum class Type {
        None,
        String,
        Number,
        Boolean,
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

    struct Function : Object {
        using Call = std::function<std::shared_ptr<Object>(Interpreter*, const std::vector<std::shared_ptr<Object>>&)>;

        Call call;
        std::size_t arity {};
    };

    inline std::shared_ptr<Object> create() {
        std::shared_ptr<None> object {std::make_shared<None>()};
        object->type = Type::None;

        return object;
    }

    inline std::shared_ptr<Object> create(const std::string& value) {
        std::shared_ptr<String> object {std::make_shared<String>()};
        object->type = Type::String;
        object->value = value;

        return object;
    }

    inline std::shared_ptr<Object> create(double value) {
        std::shared_ptr<Number> object {std::make_shared<Number>()};
        object->type = Type::Number;
        object->value = value;

        return object;
    }

    inline std::shared_ptr<Object> create(bool value) {
        std::shared_ptr<Boolean> object {std::make_shared<Boolean>()};
        object->type = Type::Boolean;
        object->value = value;

        return object;
    }

    inline std::shared_ptr<Object> create(const Function::Call& call, std::size_t arity) {
        std::shared_ptr<Function> object {std::make_shared<Function>()};
        object->type = Type::Function;
        object->call = call;
        object->arity = arity;

        return object;
    }

    template<typename T>
    std::shared_ptr<T> cast(const std::shared_ptr<Object>& object) {
        return std::static_pointer_cast<T>(object);
    }
}
