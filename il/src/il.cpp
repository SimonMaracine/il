#include "il.hpp"

#include <iostream>
#include <fstream>

#include "scanner.hpp"
#include "parser.hpp"
#include "literal.hpp"
#include "ast_printer.hpp"  // TODO temporary

void Il::run_file(const std::string& file_path) {
    const auto contents {read_file(file_path)};

    if (!contents) {
        // FIXME error
        return;
    }

    run(*contents);

    if (ctx.had_error) {
        // FIXME error
        return;
    }

    if (ctx.had_runtime_error) {
        // FIXME error
        return;
    }
}

void Il::run_repl() {
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

void Il::run(const std::string& source_code) {
    Scanner scanner {source_code, &ctx};
    const auto tokens {scanner.scan()};

    Parser parser {tokens, &ctx};

#if 0
    const auto expr {parser.parse<std::string>()};

    if (ctx.had_error) {
        return;
    }

    std::cout << AstPrinter().print(expr) << '\n';
#endif

    const auto statements {parser.parse<literal::Object>()};

    if (ctx.had_error) {
        return;
    }

    interpreter.interpret(statements);
}

std::optional<std::string> Il::read_file(const std::string& file_path) {
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