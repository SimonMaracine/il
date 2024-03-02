#include "scanner.hpp"

#include <stdexcept>
#include <cassert>
#include <utility>
#include <unordered_map>

std::vector<token::Token> Scanner::scan() {
    while (!reached_end()) {
        // Beginning of the next lexeme
        start = current;

        scan_token();
    }

    tokens.emplace_back(token::TokenType::Eof, "", line);

    return tokens;
}

void Scanner::scan_token() {
    const char character {advance()};

    // current is now pointing to the next character

    switch (character) {
        case '(':
            add_token(token::TokenType::LeftParen);
            break;
        case ')':
            add_token(token::TokenType::RightParen);
            break;
        case '{':
            add_token(token::TokenType::LeftBrace);
            break;
        case '}':
            add_token(token::TokenType::RightBrace);
            break;
        case ',':
            add_token(token::TokenType::Comma);
            break;
        case '.':
            add_token(token::TokenType::Dot);
            break;
        case ';':
            add_token(token::TokenType::Semicolon);
            break;
        case '-':
            add_token(token::TokenType::Minus);
            break;
        case '+':
            add_token(token::TokenType::Plus);
            break;
        case '/':
            if (match('/')) {
                while (peek() != '\n' && !reached_end()) {
                    advance();
                }
            } else {
                add_token(token::TokenType::Slash);
            }
            break;
        case '*':
            add_token(token::TokenType::Star);
            break;
        case '!':
            if (match('=')) {
                add_token(token::TokenType::BangEqual);
            } else {
                ctx->error(line, "Unexpected bare `!` character");
            }
            break;
        case '>':
            add_token(match('=') ? token::TokenType::GreaterEqual : token::TokenType::Greater);
            break;
        case '<':
            add_token(match('=') ? token::TokenType::LessEqual : token::TokenType::Less);
            break;
        case '=':
            add_token(match('=') ? token::TokenType::EqualEqual : token::TokenType::Equal);
            break;
        case '"':
            string();
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            number();
            break;
        case ' ':
        case '\t':
        case '\r':
            break;
        case '\n':
            line++;
            break;
        default:
            if (is_alpha(character)) {
                identifier();
            } else {
                ctx->error(line, std::string("Unexpected character: `") + character + "`");
            }
    }
}

char Scanner::advance() {
    return source_code[current++];
}

void Scanner::add_token(token::TokenType type) {
    tokens.emplace_back(type, source_code.substr(start, current - start), line);
}

void Scanner::add_token(token::TokenType type, std::string&& value) {
    tokens.emplace_back(type, source_code.substr(start, current - start), line, std::move(value));
}

void Scanner::add_token(token::TokenType type, long long value) {
    tokens.emplace_back(type, source_code.substr(start, current - start), line, value);
}

void Scanner::add_token(token::TokenType type, double value) {
    tokens.emplace_back(type, source_code.substr(start, current - start), line, value);
}

bool Scanner::match(char character) {
    if (reached_end()) {
        return false;
    }

    if (source_code[current] != character) {
        return false;
    }

    current++;
    return true;
}

char Scanner::peek() {
    if (reached_end()) {
        return 0;
    }

    return source_code[current];
}

char Scanner::peekTwo() {
    if (current + 1u >= source_code.length()) {
        return 0;
    }

    return source_code[current + 1u];
}

bool Scanner::reached_end() {
    return current >= source_code.size();
}

bool Scanner::is_digit(char character) {
    return character >= '0' && character <= '9';
}

bool Scanner::is_alpha(char character) {
    if (character >= 'A' && character <= 'Z') {
        return true;
    }

    if (character >= 'a' && character <= 'z') {
        return true;
    }

    if (character == '_') {
        return true;
    }

    return false;
}

bool Scanner::is_alpha_numeric(char character) {
    return is_alpha(character) || is_digit(character);
}

void Scanner::string() {
    while (peek() != '"' && !reached_end()) {
        if (peek() == '\n') {
            line++;
        }

        advance();
    }

    if (reached_end()) {
        ctx->error(line, "Unterminated string");
        return;
    }

    // Consume the closing quote
    advance();

    add_token(token::TokenType::String, source_code.substr(start + 1u, current - start - 2u));
}

void Scanner::number() {
    while (is_digit(peek())) {
        advance();
    }

    bool floating_point {false};

    if (peek() == '.' && is_digit(peekTwo())) {
        floating_point = true;

        // Consume the dot
        advance();

        while (is_digit(peek())) {
            advance();
        }
    }

    if (floating_point) {
        add_token(token::TokenType::Float, parse_double(source_code.substr(start, current - start)));
    } else {
        add_token(token::TokenType::Integer, parse_long_long(source_code.substr(start, current - start)));
    }
}

void Scanner::identifier() {
    while (is_alpha_numeric(peek())) {
        advance();
    }

    static const std::unordered_map<std::string, token::TokenType> KEYWORDS {
        { "let", token::TokenType::Let },
        { "true", token::TokenType::True },
        { "false", token::TokenType::False },
        { "none", token::TokenType::None },
        { "or", token::TokenType::Or },
        { "and", token::TokenType::And },
        { "not", token::TokenType::Not },
        { "if", token::TokenType::If },
        { "else", token::TokenType::Else },
        { "while", token::TokenType::While },
        { "for", token::TokenType::For },
        { "fun", token::TokenType::Fun },
        { "return", token::TokenType::Return },
        { "struct", token::TokenType::Struct }
    };

    const auto word {source_code.substr(start, current - start)};

    if (KEYWORDS.find(word) != KEYWORDS.cend()) {
        add_token(KEYWORDS.at(word));
        return;
    }

    add_token(token::TokenType::Identifier);
}

long long Scanner::parse_long_long(const std::string& string) {
    long long result {};

    try {
        result = std::stoll(string);
    } catch (const std::invalid_argument&) {
        assert(false);
    } catch (const std::out_of_range&) {
        ctx->error(line, "Number out of range");
        return 0ll;
    }

    return result;
}

double Scanner::parse_double(const std::string& string) {
    double result {};

    try {
        result = std::stod(string);
    } catch (const std::invalid_argument&) {
        assert(false);
    } catch (const std::out_of_range&) {
        ctx->error(line, "Number out of range");
        return 0.0;
    }

    return result;
}
