#include "builtins.hpp"

#include <chrono>
#include <iostream>
#include <string>
#include <cassert>

#include "ast.hpp"

namespace builtins {
    static void print_stdout(const std::vector<std::shared_ptr<object::Object>>& arguments) {
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
            case object::Type::BuiltinFunction:
                std::cout << "<builtin function>";
                break;
            case object::Type::Function:
                std::cout << "<function " << object::cast<object::Function>(value)->declaration->name.get_lexeme() << '>';
                break;
            default:
                assert(false);
                break;
        }
    }

    std::shared_ptr<object::Object> clock::call(Interpreter*, const std::vector<std::shared_ptr<object::Object>>&) {
        return object::create(
            std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count()
        );
    }

    std::size_t clock::arity() const {
        return 0u;
    }

    std::shared_ptr<object::Object> print::call(Interpreter*, const std::vector<std::shared_ptr<object::Object>>& arguments) {
        print_stdout(arguments);

        return object::create();
    }

    std::size_t print::arity() const {
        return 1u;
    }

    std::shared_ptr<object::Object> println::call(Interpreter* interpreter, const std::vector<std::shared_ptr<object::Object>>& arguments) {
        print_stdout(arguments);

        std::cout << std::endl;

        return object::create();
    }

    std::size_t println::arity() const {
        return 1u;
    }

    std::shared_ptr<object::Object> to_string::call(Interpreter*, const std::vector<std::shared_ptr<object::Object>>& arguments) {
        const auto value {arguments[0u]};

        switch (value->type) {
            case object::Type::None:
                return object::create(std::string("none"));
            case object::Type::String:
                return object::create(object::cast<object::String>(value)->value);
            case object::Type::Number:
                return object::create(std::to_string(object::cast<object::Number>(value)->value));
            case object::Type::Boolean:
                return object::create(std::string(object::cast<object::Boolean>(value)->value ? "true" : "false"));
            case object::Type::BuiltinFunction:
                return object::create(std::string("<builtin function>"));
            case object::Type::Function:
                return object::create(
                    std::string("<function " + object::cast<object::Function>(value)->declaration->name.get_lexeme() + ">")
                );
            default:
                break;
        }

        assert(false);
        return nullptr;
    }

    std::size_t to_string::arity() const {
        return 1u;
    }
}
