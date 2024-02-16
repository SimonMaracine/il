#pragma once

#include <string>
#include <optional>
#include <cstddef>

#include "token.hpp"

class IlMain;

class Context {
public:
    void error(std::size_t line, const std::string& message);
    void error(const Token& token, const std::string& message);
private:
    void report(std::size_t line, const std::string& where, const std::string& message);

    bool had_error {false};

    friend class IlMain;
};
