#pragma once

#include "object.hpp"

namespace builtins {
    struct clock : object::BuiltinFunction {
        std::shared_ptr<object::Object> call(
            Interpreter*,
            const std::vector<std::shared_ptr<object::Object>>&,
            const token::Token& token
        ) override;

        std::size_t arity() const override;
    };

    struct print : object::BuiltinFunction {
        std::shared_ptr<object::Object> call(
            Interpreter*,
            const std::vector<std::shared_ptr<object::Object>>& arguments,
            const token::Token&
        ) override;

        std::size_t arity() const override;
    };

    struct println : object::BuiltinFunction {
        std::shared_ptr<object::Object> call(
            Interpreter*,
            const std::vector<std::shared_ptr<object::Object>>& arguments,
            const token::Token&
        ) override;

        std::size_t arity() const override;
    };

    struct input : object::BuiltinFunction {
        std::shared_ptr<object::Object> call(
            Interpreter*,
            const std::vector<std::shared_ptr<object::Object>>& arguments,
            const token::Token&
        ) override;

        std::size_t arity() const override;
    };

    struct str : object::BuiltinFunction {
        std::shared_ptr<object::Object> call(
            Interpreter*,
            const std::vector<std::shared_ptr<object::Object>>& arguments,
            const token::Token&
        ) override;

        std::size_t arity() const override;
    };

    struct int_ : object::BuiltinFunction {
        std::shared_ptr<object::Object> call(
            Interpreter*,
            const std::vector<std::shared_ptr<object::Object>>& arguments,
            const token::Token& token
        ) override;
        std::size_t arity() const override;
    };

    struct float_ : object::BuiltinFunction {
        std::shared_ptr<object::Object> call(
            Interpreter*,
            const std::vector<std::shared_ptr<object::Object>>& arguments,
            const token::Token& token
        ) override;

        std::size_t arity() const override;
    };

    struct bool_ : object::BuiltinFunction {
        std::shared_ptr<object::Object> call(
            Interpreter*,
            const std::vector<std::shared_ptr<object::Object>>& arguments,
            const token::Token& token
        ) override;

        std::size_t arity() const override;
    };
}
