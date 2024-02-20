#include "environment.hpp"

#include "runtime_error.hpp"

void Environment::define(const std::string& name, const literal::Object& value) {
    values[name] = value;
}

const literal::Object Environment::get(const Token& name) const {
    if (values.find(name.get_lexeme()) != values.cend()) {
        return values.at(name.get_lexeme());
    }

    throw RuntimeError(name, "Undefined variable `" + name.get_lexeme() + "`");
}
