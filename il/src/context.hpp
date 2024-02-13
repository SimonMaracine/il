#pragma once

#include <string>
#include <optional>
#include <cstddef>

class IlInterpreter;

class Context {
public:
    void error(std::size_t line, const std::string& message);
private:
    void report(std::size_t line, const std::string& where, const std::string& message);

    bool had_error {false};

    friend class IlInterpreter;
};
