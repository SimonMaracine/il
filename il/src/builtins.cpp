#include "builtins.hpp"

#include <chrono>
#include <iostream>
#include <string>
#include <cassert>

#include "ast.hpp"

namespace builtins {
    static void print_stdout(const std::vector<std::shared_ptr<object::Object>>& arguments) {
        const auto value {arguments[0u]};

        std::cout << value->to_string();
    }

    std::shared_ptr<object::Object> clock::call(Interpreter*, const std::vector<std::shared_ptr<object::Object>>&) {
        return object::create_float(
            std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count()
        );
    }

    std::size_t clock::arity() const {
        return 0u;
    }

    std::shared_ptr<object::Object> print::call(Interpreter*, const std::vector<std::shared_ptr<object::Object>>& arguments) {
        print_stdout(arguments);

        return object::create_none();
    }

    std::size_t print::arity() const {
        return 1u;
    }

    std::shared_ptr<object::Object> println::call(Interpreter*, const std::vector<std::shared_ptr<object::Object>>& arguments) {
        print_stdout(arguments);

        std::cout << std::endl;

        return object::create_none();
    }

    std::size_t println::arity() const {
        return 1u;
    }

    std::shared_ptr<object::Object> to_string::call(Interpreter*, const std::vector<std::shared_ptr<object::Object>>& arguments) {
        const auto value {arguments[0u]};

        return object::create_string(value->to_string());
    }

    std::size_t to_string::arity() const {
        return 1u;
    }
}
