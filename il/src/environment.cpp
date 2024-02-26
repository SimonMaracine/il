#include "environment.hpp"

#include "runtime_error.hpp"

void Environment::define(const std::string& name, std::shared_ptr<object::Object> value) {
    values[name] = value;
}

std::shared_ptr<object::Object> Environment::get(const token::Token& name) const {
    if (values.find(name.get_lexeme()) != values.cend()) {
        return values.at(name.get_lexeme());
    }

    if (enclosing != nullptr) {
        return enclosing->get(name);
    }

    throw RuntimeError(name, "Undefined variable `" + name.get_lexeme() + "`");
}

void Environment::assign(const token::Token& name, std::shared_ptr<object::Object> value) {
    if (values.find(name.get_lexeme()) != values.cend()) {
        values.at(name.get_lexeme()) = value;
        return;
    }

    if (enclosing != nullptr) {
        enclosing->assign(name, value);
        return;
    }

    throw RuntimeError(name, "Undefined variable `" + name.get_lexeme() + "`");
}
