#pragma once

#include "object.hpp"

namespace builtins {
    std::shared_ptr<object::Object> clock(Interpreter*, const std::vector<std::shared_ptr<object::Object>>&);
    std::shared_ptr<object::Object> print(Interpreter*, const std::vector<std::shared_ptr<object::Object>>& arguments);
    std::shared_ptr<object::Object> println(Interpreter* interpreter, const std::vector<std::shared_ptr<object::Object>>& arguments);
    std::shared_ptr<object::Object> to_string(Interpreter*, const std::vector<std::shared_ptr<object::Object>>& arguments);
}
