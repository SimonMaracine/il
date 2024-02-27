#pragma once

#include <string>
#include <optional>

#include "context.hpp"
#include "interpreter.hpp"

class Il {
public:
    Il()
        : interpreter(&ctx) {}

    int run_file(const std::string& file_path);
    int run_repl();
private:
    void run(const std::string& source_code);
    std::optional<std::string> read_file(const std::string& file_path);

    Context ctx;
    Interpreter interpreter;
};
