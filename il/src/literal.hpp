#pragma once

#include <variant>
#include <string>

namespace literal {
    struct Null {};
    using Literal = std::variant<Null, std::string, double, bool>;
    using Object = Literal;
}
