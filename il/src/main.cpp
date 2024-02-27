#include "il.hpp"

int main(int argc, char** argv) {
    Il interpreter;

    if (argc == 1) {
        return interpreter.run_repl();
    } else {
        // TODO the other arguments should be picked up by the script
        return interpreter.run_file(argv[1u]);
    }
}
