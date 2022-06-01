#include <optional>
#include <vector>
#include <string>
#include <iostream>

#include "lexer.h"
#include "token.h"

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    advance();

    while (current_character.has_value()) {
        switch (current_character.value()) {
            case ' ':
            case '\n':
            case '\r':
            case '\t':
                advance();
                break;
            case '+':
                tokens.push_back(Token(TokenType::Plus));
                advance();
                break;
            case '-':
                tokens.push_back(Token(TokenType::Minus));
                advance();
                break;
            case '*':
                tokens.push_back(Token(TokenType::Mul));
                advance();
                break;
            case '/':
                tokens.push_back(Token(TokenType::Div));
                advance();
                break;
            case '(':
                tokens.push_back(Token(TokenType::LeftParen));
                advance();
                break;
            case ')':
                tokens.push_back(Token(TokenType::RightParen));
                advance();
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
                tokens.push_back(build_number());
                advance();
                break;
            default:
                std::cout << "Invalid character" << std::endl;
                exit(1);
        }
    }

    return tokens;
}

void Lexer::advance() {
    if (position < text.size()) {
        current_character = std::make_optional<char>(text[position]);
    } else {
        current_character = std::nullopt;
    }

    position++;
}

Token Lexer::build_number() {
    std::string number_string;
    unsigned int dot_count = 0;

    while (current_character.has_value()) {
        switch (current_character.value()) {
            case '.':
                dot_count++;
                number_string += current_character.value();
                advance();
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
                number_string += current_character.value();
                advance();
                break;
            default:
                goto out_of_loop;
        }

        if (dot_count > 1) {
            std::cout << "Invalid syntax: too many decimal points in number" << std::endl;
            exit(1);
        }
    }

    out_of_loop:
    Token token;

    if (dot_count == 0) {
        token.type = TokenType::Int;
        token.value.integer = std::stoi(number_string);
        return token;
    } else {
        token.type = TokenType::Float;
        token.value.real = std::stof(number_string);
        return token;
    }
}
