#pragma once

#include <string>
#include <optional>
#include <cstddef>

class IlInterpreter {
public:
    void run_file(const std::string& file_path);  // TODO maybe return code
    void run_repl();
private:
    void run(const std::string& source_code);
    std::optional<std::string> read_file(const std::string& file_path);
    void report(std::size_t line, const std::string& where, const std::string& message);
    void error(std::size_t line, const std::string& message);

    bool had_error {false};
};
