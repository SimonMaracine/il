#pragma once

#include <string>
#include <memory>
#include <utility>

namespace object {
    enum class Type {
        None,
        String,
        Number,
        Boolean
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

    template<typename T>
    std::shared_ptr<T> cast(const std::shared_ptr<Object>& object) {
        return std::static_pointer_cast<T>(object);
    }
}
