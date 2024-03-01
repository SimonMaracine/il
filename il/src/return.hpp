#pragma once

#include <memory>

#include "object.hpp"

struct Return {
    explicit Return(std::shared_ptr<object::Object> value)
        : value(value) {}

    std::shared_ptr<object::Object> value;
};
