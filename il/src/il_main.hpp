#pragma once

#include <string>
#include <optional>

#include "context.hpp"
#include "interpreter.hpp"

class IlMain {
public:
    IlMain()
        : interpreter(&ctx) {}

    void run_file(const std::string& file_path);  // TODO maybe return code
    void run_repl();
private:
    void run(const std::string& source_code);
    std::optional<std::string> read_file(const std::string& file_path);

    Context ctx;
    Interpreter interpreter;
};
