#include "interpreter.hpp"

#include <iostream>
#include <fstream>

#include "scanner.hpp"
#include "parser.hpp"
#include "ast_printer.hpp"  // TODO temporary

void IlInterpreter::run_file(const std::string& file_path) {
    const auto contents {read_file(file_path)};

    if (!contents) {
        return;  // FIXME error
    }

    run(*contents);

    if (ctx.had_error) {
        // FIXME error
    }
}

void IlInterpreter::run_repl() {
    while (true) {
        std::cout << "il> ";

        std::string line;
        std::getline(std::cin, line);

        if (std::cin.eof()) {
            std::cout << std::endl;
            break;
        }

        if (std::cin.bad()) {
            std::cin.clear();
            continue;
        }

        if (line.empty()) {
            continue;
        }

        run(line);

        ctx.had_error = false;
    }
}

void IlInterpreter::run(const std::string& source_code) {
    Scanner scanner {source_code, ctx};
    const auto tokens {scanner.scan()};

    Parser parser {tokens, ctx};
    const auto expr {parser.parse()};

    if (ctx.had_error) {
        return;
    }

    // FIXME
    // std::cout << AstPrinter().print(expr) << '\n';
}

std::optional<std::string> IlInterpreter::read_file(const std::string& file_path) {
    std::ifstream stream {file_path, std::ios_base::binary};

    if (!stream.is_open()) {
        return std::nullopt;
    }

    stream.seekg(0, stream.end);
    const auto size {stream.tellg()};
    stream.seekg(0, stream.beg);

    char* buffer {new char[size]};
    stream.read(buffer, size);

    const std::string contents {buffer, static_cast<std::size_t>(size)};

    delete[] buffer;

    return contents;
}
