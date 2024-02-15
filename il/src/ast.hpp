#pragma once

#include <memory>

#include "token.hpp"

// TODO use preprocessor to generate code

template<typename R>
struct Expr;

template<typename R>
struct Literal;

template<typename R>
struct Grouping;

template<typename R>
struct Unary;

template<typename R>
struct Binary;

template<typename R>
struct Visitor {
    virtual R visit(Literal<R>* expr) = 0;
    virtual R visit(Grouping<R>* expr) = 0;
    virtual R visit(Unary<R>* expr) = 0;
    virtual R visit(Binary<R>* expr) = 0;
};

template<typename R>
struct Expr {
    virtual ~Expr() noexcept = default;

    virtual R accept(Visitor<R>* visitor) = 0;
};

template<typename R>
struct Literal : Expr<R> {
    R accept(Visitor<R>* visitor) override {
        return visitor->visit(this);
    }

    // TODO value
};

template<typename R>
struct Grouping : Expr<R> {
    Grouping(std::shared_ptr<Expr<R>> expression)
        : expression(expression) {}

    R accept(Visitor<R>* visitor) override {
        return visitor->visit(this);
    }

    std::shared_ptr<Expr<R>> expression;
};

template<typename R>
struct Unary : Expr<R> {
    Unary(const Token& operator_, std::shared_ptr<Expr<R>> right)
        : operator_(operator_), right(right) {}

    R accept(Visitor<R>* visitor) override {
        return visitor->visit(this);
    }

    Token operator_;
    std::shared_ptr<Expr<R>> right;
};

template<typename R>
struct Binary : Expr<R> {
    Binary(std::shared_ptr<Expr<R>> left, const Token& operator_, std::shared_ptr<Expr<R>> right)
        : left(left), operator_(operator_), right(right) {}

    R accept(Visitor<R>* visitor) override {
        return visitor->visit(this);
    }

    std::shared_ptr<Expr<R>> left;
    Token operator_;
    std::shared_ptr<Expr<R>> right;
};
