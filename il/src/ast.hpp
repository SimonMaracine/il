#pragma once

#include <memory>
#include <vector>

#include "token.hpp"
#include "object.hpp"

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
        struct Logical;

        template<typename R>
        struct Call;

        template<typename R>
        struct Get;

        template<typename R>
        struct Visitor {
            virtual R visit(Literal<R>* expr) = 0;
            virtual R visit(Grouping<R>* expr) = 0;
            virtual R visit(Unary<R>* expr) = 0;
            virtual R visit(Binary<R>* expr) = 0;
            virtual R visit(Variable<R>* expr) = 0;
            virtual R visit(Assignment<R>* expr) = 0;
            virtual R visit(Logical<R>* expr) = 0;
            virtual R visit(Call<R>* expr) = 0;
            virtual R visit(Get<R>* expr) = 0;
        };

        template<typename R>
        struct Expr {
            virtual ~Expr() noexcept = default;

            virtual R accept(Visitor<R>* visitor) = 0;
        };

        template<typename R>
        struct Literal : Expr<R> {
            Literal(std::shared_ptr<object::Object> value)
                : value(value) {}

            R accept(Visitor<R>* visitor) override {
                return visitor->visit(this);
            }

            std::shared_ptr<object::Object> value;
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
            Unary(const token::Token& operator_, std::shared_ptr<Expr<R>> right)
                : operator_(operator_), right(right) {}

            R accept(Visitor<R>* visitor) override {
                return visitor->visit(this);
            }

            token::Token operator_;
            std::shared_ptr<Expr<R>> right;
        };

        template<typename R>
        struct Binary : Expr<R> {
            Binary(std::shared_ptr<Expr<R>> left, const token::Token& operator_, std::shared_ptr<Expr<R>> right)
                : left(left), operator_(operator_), right(right) {}

            R accept(Visitor<R>* visitor) override {
                return visitor->visit(this);
            }

            std::shared_ptr<Expr<R>> left;
            token::Token operator_;
            std::shared_ptr<Expr<R>> right;
        };

        template<typename R>
        struct Variable : Expr<R> {
            Variable(const token::Token& name)
                : name(name) {}

            R accept(Visitor<R>* visitor) override {
                return visitor->visit(this);
            }

            token::Token name;
        };

        template<typename R>
        struct Assignment : Expr<R> {
            Assignment(const token::Token& name, std::shared_ptr<Expr<R>> value)
                : name(name), value(value) {}

            R accept(Visitor<R>* visitor) override {
                return visitor->visit(this);
            }

            token::Token name;
            std::shared_ptr<Expr<R>> value;
        };

        template<typename R>
        struct Logical : Expr<R> {
            Logical(std::shared_ptr<Expr<R>> left, const token::Token& operator_, std::shared_ptr<Expr<R>> right)
                : left(left), operator_(operator_), right(right) {}

            R accept(Visitor<R>* visitor) override {
                return visitor->visit(this);
            }

            std::shared_ptr<Expr<R>> left;
            token::Token operator_;
            std::shared_ptr<Expr<R>> right;
        };

        template<typename R>
        struct Call : Expr<R> {
            Call(std::shared_ptr<Expr<R>> callee, const token::Token& paren, const std::vector<std::shared_ptr<Expr<R>>>& arguments)
                : callee(callee), paren(paren), arguments(arguments) {}

            R accept(Visitor<R>* visitor) override {
                return visitor->visit(this);
            }

            std::shared_ptr<Expr<R>> callee;
            token::Token paren;
            std::vector<std::shared_ptr<Expr<R>>> arguments;
        };

        template<typename R>
        struct Get : Expr<R> {
            Get(std::shared_ptr<Expr<R>> object, const token::Token& name)
                : object(object), name(name) {}

            R accept(Visitor<R>* visitor) override {
                return visitor->visit(this);
            }

            std::shared_ptr<Expr<R>> object;
            token::Token name;
        };
    }

    namespace stmt {
        using namespace expr;

        template<typename R>
        struct Expression;

        template<typename R>
        struct Let;

        template<typename R>
        struct Function;

        template<typename R>
        struct Struct;

        template<typename R>
        struct If;

        template<typename R>
        struct While;

        template<typename R>
        struct Block;

        template<typename R>
        struct Return;

        template<typename R>
        struct Visitor {
            virtual R visit(const Expression<R>* stmt) = 0;
            virtual R visit(const Let<R>* stmt) = 0;
            virtual R visit(const Function<R>* stmt) = 0;
            virtual R visit(const Struct<R>* stmt) = 0;
            virtual R visit(const If<R>* stmt) = 0;
            virtual R visit(const While<R>* stmt) = 0;
            virtual R visit(const Block<R>* stmt) = 0;
            virtual R visit(const Return<R>* stmt) = 0;
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
        struct Let : Stmt<R> {
            Let(const token::Token& name, std::shared_ptr<Expr<R>> initializer)
                : name(name), initializer(initializer) {}

            R accept(Visitor<R>* visitor) override {
                return visitor->visit(this);
            }

            token::Token name;
            std::shared_ptr<Expr<R>> initializer;
        };

        template<typename R>
        struct Function : Stmt<R> {
            Function(const token::Token& name, const std::vector<token::Token>& parameters, const std::vector<std::shared_ptr<Stmt<R>>>& body)
                : name(name), parameters(parameters), body(body) {}

            R accept(Visitor<R>* visitor) override {
                return visitor->visit(this);
            }

            token::Token name;
            std::vector<token::Token> parameters;
            std::vector<std::shared_ptr<Stmt<R>>> body;
        };

        template<typename R>
        struct Struct : Stmt<R> {
            Struct(const token::Token& name, const std::vector<std::shared_ptr<Function<R>>>& methods)
                : name(name), methods(methods) {}

            R accept(Visitor<R>* visitor) override {
                return visitor->visit(this);
            }

            token::Token name;
            std::vector<std::shared_ptr<Function<R>>> methods;
        };

        template<typename R>
        struct If : Stmt<R> {
            If(std::shared_ptr<Expr<R>> condition, std::shared_ptr<Stmt<R>> then_branch, std::shared_ptr<Stmt<R>> else_branch, const token::Token& paren)
                : condition(condition), then_branch(then_branch), else_branch(else_branch), paren(paren) {}

            R accept(Visitor<R>* visitor) override {
                return visitor->visit(this);
            }

            std::shared_ptr<Expr<R>> condition;
            std::shared_ptr<Stmt<R>> then_branch;
            std::shared_ptr<Stmt<R>> else_branch;
            token::Token paren;
        };

        template<typename R>
        struct While : Stmt<R> {
            While(std::shared_ptr<Expr<R>> condition, std::shared_ptr<Stmt<R>> body, const token::Token& paren)
                : condition(condition), body(body), paren(paren) {}

            R accept(Visitor<R>* visitor) override {
                return visitor->visit(this);
            }

            std::shared_ptr<Expr<R>> condition;
            std::shared_ptr<Stmt<R>> body;
            token::Token paren;
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

        template<typename R>
        struct Return : Stmt<R> {
            Return(const token::Token& keyword, std::shared_ptr<Expr<R>> value)
                : keyword(keyword), value(value) {}

            R accept(Visitor<R>* visitor) override {
                return visitor->visit(this);
            }

            token::Token keyword;
            std::shared_ptr<Expr<R>> value;
        };
    }
}
