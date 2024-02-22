#pragma once

#include <unordered_map>
#include <string>

#include "literal.hpp"
#include "token.hpp"

class Environment {
public:
    Environment() = default;

    Environment(Environment* enclosing)
        : enclosing(enclosing) {}

    void define(const std::string& name, const literal::Object& value = literal::Null());
    literal::Object get(const Token& name) const;
    void assign(const Token& name, const literal::Object& value);
private:
    std::unordered_map<std::string, literal::Object> values;
    Environment* enclosing {nullptr};
};
