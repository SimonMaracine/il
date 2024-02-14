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
    std::shared_ptr<Expr<R>> accept(Visitor<R>* visitor) override {
        return visitor->visit(this);
    }

    // TODO value
};

template<typename R>
struct Grouping : Expr<R> {
    std::shared_ptr<Expr<R>> accept(Visitor<R>* visitor) override {
        return visitor->visit(this);
    }

    std::shared_ptr<Expr<R>> expression;
};

template<typename R>
struct Unary : Expr<R> {
    std::shared_ptr<Expr<R>> accept(Visitor<R>* visitor) override {
        return visitor->visit(this);
    }

    Token operat;
    std::shared_ptr<Expr<R>> expression;
};

template<typename R>
struct Binary : Expr<R> {
    std::shared_ptr<Expr<R>> accept(Visitor<R>* visitor) override {
        return visitor->visit(this);
    }

    std::shared_ptr<Expr<R>> left;
    Token operat;
    std::shared_ptr<Expr<R>> right;
};
