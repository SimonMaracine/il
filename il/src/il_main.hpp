#pragma once

#include <string>
#include <optional>

#include "context.hpp"

class IlMain {
public:
    void run_file(const std::string& file_path);  // TODO maybe return code
    void run_repl();
private:
    void run(const std::string& source_code);
    std::optional<std::string> read_file(const std::string& file_path);

    Context ctx;
};
