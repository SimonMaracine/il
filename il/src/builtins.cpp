#include "builtins.hpp"

#include <chrono>
#include <iostream>
#include <string>
#include <cassert>
#include <stdexcept>

#include "ast.hpp"
#include "interpreter.hpp"
#include "runtime_error.hpp"

namespace builtins {
    static long long parse_long_long(const std::string& string, const token::Token& token) {
        long long result {};

        try {
            result = std::stoll(string);
        } catch (const std::invalid_argument&) {
            throw RuntimeError(token, "Invalid integer value");
        } catch (const std::out_of_range&) {
            throw RuntimeError(token, "Integer value out of range");
        }

        return result;
    }

    static double parse_double(const std::string& string, const token::Token& token) {
        double result {};

        try {
            result = std::stod(string);
        } catch (const std::invalid_argument&) {
            throw RuntimeError(token, "Invalid float value");
        } catch (const std::out_of_range&) {
            throw RuntimeError(token, "Float value out of range");
        }

        return result;
    }

    std::shared_ptr<object::Object> clock::call(
        Interpreter*,
        const std::vector<std::shared_ptr<object::Object>>&,
        const token::Token&
    ) {
        return object::create_float(
            std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count()
        );
    }

    std::size_t clock::arity() const {
        return 0u;
    }

    std::shared_ptr<object::Object> print::call(
        Interpreter*,
        const std::vector<std::shared_ptr<object::Object>>& arguments,
        const token::Token&
    ) {
        std::cout << arguments[0u]->to_string();

        return object::create_none();
    }

    std::size_t print::arity() const {
        return 1u;
    }

    std::shared_ptr<object::Object> println::call(
        Interpreter*,
        const std::vector<std::shared_ptr<object::Object>>& arguments,
        const token::Token&
    ) {
        std::cout << arguments[0u]->to_string() << std::endl;

        return object::create_none();
    }

    std::size_t println::arity() const {
        return 1u;
    }

    std::shared_ptr<object::Object> str::call(
        Interpreter*,
        const std::vector<std::shared_ptr<object::Object>>& arguments,
        const token::Token&
    ) {
        return object::create_string(arguments[0u]->to_string());
    }

    std::size_t str::arity() const {
        return 1u;
    }

    std::shared_ptr<object::Object> int_::call(
        Interpreter*,
        const std::vector<std::shared_ptr<object::Object>>& arguments,
        const token::Token& token
    ) {
        auto argument {arguments[0u]};

        switch (argument->type) {
            case object::Type::String:
                return object::create_integer(
                    parse_long_long(object::cast<object::String>(argument)->value, token)
                );
            case object::Type::Integer:
                return object::create_integer(
                    object::cast<object::Integer>(argument)->value
                );
            case object::Type::Float:
                return object::create_integer(
                    static_cast<long long>(object::cast<object::Float>(argument)->value)
                );
            case object::Type::Boolean:
                return object::create_integer(
                    static_cast<long long>(object::cast<object::Boolean>(argument)->value)
                );
            default:
                throw RuntimeError(token, "int() argument must be either string, integer, float or boolean");
        }

        assert(false);
        return nullptr;
    }

    std::size_t int_::arity() const {
        return 1u;
    }

    std::shared_ptr<object::Object> float_::call(
        Interpreter*,
        const std::vector<std::shared_ptr<object::Object>>& arguments,
        const token::Token& token
    ) {
        auto argument {arguments[0u]};

        switch (argument->type) {
            case object::Type::String:
                return object::create_float(
                    parse_double(object::cast<object::String>(argument)->value, token)
                );
            case object::Type::Integer:
                return object::create_float(
                    static_cast<double>(object::cast<object::Integer>(argument)->value)
                );
            case object::Type::Float:
                return object::create_float(
                    object::cast<object::Float>(argument)->value
                );
            case object::Type::Boolean:
                return object::create_float(
                    static_cast<double>(object::cast<object::Boolean>(argument)->value)
                );
            default:
                throw RuntimeError(token, "float() argument must be either string, integer, float or boolean");
        }

        assert(false);
        return nullptr;
    }

    std::size_t float_::arity() const {
        return 1u;
    }

    std::shared_ptr<object::Object> bool_::call(
        Interpreter*,
        const std::vector<std::shared_ptr<object::Object>>& arguments,
        const token::Token& token
    ) {
        auto argument {arguments[0u]};

        switch (argument->type) {
            case object::Type::None:
                return object::create_bool(false);
            case object::Type::String:
                return object::create_bool(
                    object::cast<object::String>(argument)->value.empty()
                );
            case object::Type::Integer:
                return object::create_bool(
                    object::cast<object::Integer>(argument)->value
                );
            case object::Type::Float:
                return object::create_bool(
                    object::cast<object::Float>(argument)->value
                );
            case object::Type::Boolean:
                return object::create_bool(
                    object::cast<object::Boolean>(argument)->value
                );
            default:
                throw RuntimeError(token, "bool() argument must be either none, string, integer, float or boolean");
        }

        assert(false);
        return nullptr;
    }

    std::size_t bool_::arity() const {
        return 1u;
    }

    std::shared_ptr<object::Object> input::call(
        Interpreter*,
        const std::vector<std::shared_ptr<object::Object>>& arguments,
        const token::Token&
    ) {
        std::cout << arguments[0u]->to_string();

        std::string buffer;
        std::getline(std::cin, buffer);

        if (std::cin.eof()) {
            std::cin.clear();
            return object::create_string("");
        }

        if (std::cin.bad()) {
            std::cin.clear();
            return object::create_string("");
        }

        return object::create_string(buffer);
    }

    std::size_t input::arity() const {
        return 1u;
    }
}
