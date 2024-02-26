#pragma once

#include <unordered_map>
#include <string>
#include <memory>

#include "object.hpp"
#include "token.hpp"

class Environment {
public:
    Environment() = default;

    Environment(Environment* enclosing)
        : enclosing(enclosing) {}

    void define(const std::string& name, std::shared_ptr<object::Object> value);
    std::shared_ptr<object::Object> get(const token::Token& name) const;
    void assign(const token::Token& name, std::shared_ptr<object::Object> value);
private:
    std::unordered_map<std::string, std::shared_ptr<object::Object>> values;
    Environment* enclosing {nullptr};
};
