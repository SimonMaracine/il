#include "builtins.hpp"

#include <chrono>

namespace builtins {
    std::shared_ptr<object::Object> clock::call(Interpreter*, const std::vector<std::shared_ptr<object::Object>>&) {
        return object::create(
            std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count()
        );
    }

    std::size_t clock::arity() const {
        return 0u;
    }
}
