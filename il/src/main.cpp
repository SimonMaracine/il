#include <iostream>
#include <cstdlib>

#include "interpreter.hpp"

int main(int argc, char** argv) {
    IlInterpreter interpreter;

    if (argc > 2) {
        std::cout << "Invalid arguments\n";
        std::exit(1);
    } else if (argc == 2) {
        interpreter.run_file(argv[1u]);
    } else {
        interpreter.run_repl();
    }
}
