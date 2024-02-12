#pragma once

#include <vector>
#include <string>

#include "token.hpp"

class Scanner {
public:
    Scanner(const std::string& source_code)
        : source_code(source_code) {}

    std::vector<Token> scan() const;
private:
    std::string source_code;
};
