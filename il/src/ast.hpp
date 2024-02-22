#pragma once

#include <memory>
#include <vector>

#include "token.hpp"
#include "literal.hpp"

namespace ast {
    namespace expr {
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
        struct Variable;

        template<typename R>
        struct Assignment;

        template<typename R>
        struct Visitor {
            virtual R visit(Literal<R>* expr) = 0;
            virtual R visit(Grouping<R>* expr) = 0;
            virtual R visit(Unary<R>* expr) = 0;
            virtual R visit(Binary<R>* expr) = 0;
            virtual R visit(Variable<R>* expr) = 0;
            virtual R visit(Assignment<R>* expr) = 0;
        };

        template<typename R>
        struct Expr {
            virtual ~Expr() noexcept = default;

            virtual R accept(Visitor<R>* visitor) = 0;
        };

        template<typename R>
        struct Literal : Expr<R> {
            Literal(const literal::Object& value)
                : value(value) {}

            R accept(Visitor<R>* visitor) override {
                return visitor->visit(this);
            }

            literal::Object value;
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

        template<typename R>
        struct Variable : Expr<R> {
            Variable(const Token& name)
                : name(name) {}

            R accept(Visitor<R>* visitor) override {
                return visitor->visit(this);
            }

            Token name;
        };

        template<typename R>
        struct Assignment : Expr<R> {
            Assignment(const Token& name, std::shared_ptr<Expr<R>> value)
                : name(name), value(value) {}

            R accept(Visitor<R>* visitor) override {
                return visitor->visit(this);
            }

            Token name;
            std::shared_ptr<Expr<R>> value;
        };
    }

    namespace stmt {
        using namespace expr;

        template<typename R>
        struct Expression;

        template<typename R>
        struct Print;

        template<typename R>
        struct Let;

        template<typename R>
        struct Block;

        template<typename R>
        struct Visitor {
            virtual R visit(const Expression<R>* stmt) = 0;
            virtual R visit(const Print<R>* stmt) = 0;
            virtual R visit(const Let<R>* stmt) = 0;
            virtual R visit(const Block<R>* stmt) = 0;
        };

        template<typename R>
        struct Stmt {
            virtual ~Stmt() noexcept = default;

            virtual R accept(Visitor<R>* visitor) = 0;
        };

        template<typename R>
        struct Expression : Stmt<R> {
            Expression(std::shared_ptr<Expr<R>> expression)
                : expression(expression) {}

            R accept(Visitor<R>* visitor) override {
                return visitor->visit(this);
            }

            std::shared_ptr<Expr<R>> expression;
        };

        template<typename R>
        struct Print : Stmt<R> {
            Print(std::shared_ptr<Expr<R>> expression)
                : expression(expression) {}

            R accept(Visitor<R>* visitor) override {
                return visitor->visit(this);
            }

            std::shared_ptr<Expr<R>> expression;
        };

        template<typename R>
        struct Let : Stmt<R> {
            Let(const Token& name, std::shared_ptr<Expr<R>> initializer)
                : name(name), initializer(initializer) {}

            R accept(Visitor<R>* visitor) override {
                return visitor->visit(this);
            }

            Token name;
            std::shared_ptr<Expr<R>> initializer;
        };

        template<typename R>
        struct Block : Stmt<R> {
            Block(const std::vector<std::shared_ptr<Stmt<R>>>& statements)
                : statements(statements) {}

            R accept(Visitor<R>* visitor) override {
                return visitor->visit(this);
            }

            std::vector<std::shared_ptr<Stmt<R>>> statements;
        };
    }
}
