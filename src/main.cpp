#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"

std::string read_file(std::ifstream&& file) {
    std::string contents;
    std::string line;

    while (std::getline(file, line)) {
        contents += line + "\n";
    }

    return contents;
}

int main(int argc, char** argv) {
    if (argc < 2 || argc > 2) {
        std::cout << "Invalid arguments passed to the interpreter" << std::endl;
        exit(1);
    }

    std::ifstream file (argv[1]);

    if (!file.is_open()) {
        std::cout << "Error opening source file" << std::endl;
        exit(1);
    }

    std::string contents = read_file(std::move(file));
    
    Lexer lexer (std::move(contents));
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser (std::move(tokens));
    std::shared_ptr<Node> ast = parser.parse();

    if (ast != nullptr) {
        std::cout << *ast << std::endl;
    }
}
