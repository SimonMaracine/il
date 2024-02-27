#include "il.hpp"

#include <iostream>
#include <fstream>
#include <cstddef>
#include <memory>

#include "scanner.hpp"
#include "parser.hpp"
#include "object.hpp"
#include "ast_printer.hpp"  // TODO temporary

int Il::run_file(const std::string& file_path) {
    const auto contents {read_file(file_path)};

    if (!contents) {
        std::cerr << "il: could not read file `" + file_path + "`\n";
        return 1;
    }

    run(*contents);

    if (ctx.had_error) {
        return 1;
    }

    if (ctx.had_runtime_error) {
        return 1;
    }

    return 0;
}

int Il::run_repl() {
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

    return 0;
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

    const auto statements {parser.parse<std::shared_ptr<object::Object>>()};

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
