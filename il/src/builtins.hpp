#pragma once

#include "object.hpp"

namespace builtins {
    struct clock : object::BuiltinFunction {
        std::shared_ptr<object::Object> call(Interpreter*, const std::vector<std::shared_ptr<object::Object>>&) override;
        std::size_t arity() const override;
    };

    struct print : object::BuiltinFunction {
        std::shared_ptr<object::Object> call(Interpreter*, const std::vector<std::shared_ptr<object::Object>>& arguments) override;
        std::size_t arity() const override;
    };

    struct println : object::BuiltinFunction {
        std::shared_ptr<object::Object> call(Interpreter*, const std::vector<std::shared_ptr<object::Object>>& arguments) override;
        std::size_t arity() const override;
    };

    struct to_string : object::BuiltinFunction {
        std::shared_ptr<object::Object> call(Interpreter*, const std::vector<std::shared_ptr<object::Object>>& arguments) override;
        std::size_t arity() const override;
    };
}
