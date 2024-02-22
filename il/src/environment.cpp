#include "environment.hpp"

#include "runtime_error.hpp"

void Environment::define(const std::string& name, const literal::Object& value) {
    values[name] = value;
}

literal::Object Environment::get(const Token& name) const {
    if (values.find(name.get_lexeme()) != values.cend()) {
        return values.at(name.get_lexeme());
    }

    throw RuntimeError(name, "Undefined variable `" + name.get_lexeme() + "`");
}

void Environment::assign(const Token& name, const literal::Object& value) {
    if (values.find(name.get_lexeme()) != values.cend()) {
        values.at(name.get_lexeme()) = value;
        return;
    }

    throw RuntimeError(name, "Undefined variable `" + name.get_lexeme() + "`");
}
