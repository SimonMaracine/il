#pragma once

#include "object.hpp"

namespace builtins {
    struct clock : object::Object, object::Callable {
        std::shared_ptr<object::Object> call(Interpreter*, const std::vector<std::shared_ptr<object::Object>>&) override;
        std::size_t arity() const override;
    };
}
