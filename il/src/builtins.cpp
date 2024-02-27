#include "builtins.hpp"

#include <chrono>
#include <iostream>
#include <cassert>

namespace builtins {
    std::shared_ptr<object::Object> clock(Interpreter*, const std::vector<std::shared_ptr<object::Object>>&) {
        return object::create(
            std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count()
        );
    }

    std::shared_ptr<object::Object> print(Interpreter*, const std::vector<std::shared_ptr<object::Object>>& arguments) {
        const auto value {arguments[0u]};

        switch (value->type) {
            case object::Type::None:
                std::cout << "none";
                break;
            case object::Type::String:
                std::cout << object::cast<object::String>(value)->value;
                break;
            case object::Type::Number:
                std::cout << object::cast<object::Number>(value)->value;
                break;
            case object::Type::Boolean:
                std::cout << std::boolalpha << object::cast<object::Boolean>(value)->value;
                break;
            case object::Type::Function:
                std::cout << "<builtin function>";
                break;
            default:
                assert(false);
                break;
        }

        return object::create();
    }

    std::shared_ptr<object::Object> println(Interpreter* interpreter, const std::vector<std::shared_ptr<object::Object>>& arguments) {
        const auto result {print(interpreter, arguments)};

        std::cout << std::endl;

        return result;
    }
}
