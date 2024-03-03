# IL lang

## Yet Another Programming Language

As of March 3, 2024, there are exactly 686 programming languages listed on the
[List of programming languages](https://en.wikipedia.org/wiki/List_of_programming_languages) article on Wikipedia.
Clearly we need a lot more languages, as 686 is not enough.

IL lang is an imperative, procedural, scripting, toy programming language made for learning purposes. *Scripting* is
maybe a little too much said, as it doesn't offer all the necessary features needed for a full-fledged scripting
language. To be more exact, the core language features (data types, functions, stucts, control flow) are somewhat
sufficient, but its standard library, builtin functions, is severely lacking, and it's also missing a foreign
function interface (FFI).

*IL* here stands for *Interpreted Language*. Although IL is *implemented* as an interpreted language, it could very
well actually be a compiled language to bytecode or even to machine code. *How* it's implemented has nothing to
do with the actual *language*.

This programming language is based on the second part of the amazing book
[Crafting Interpreters](https://craftinginterpreters.com/) by Robert Nystrom. IL is very similar to the language
created in the book, but still it ended up different from it in some regards. I'll talk about the differences in
a later section.

## Core Language Features And Syntax

IL, being just a hobby language, is not that big and it's far, far from complete. It features basic data types like
integers, floats, booleans, strings, none (null type), but also aggregate types: structs. Because it is an imperative
language, at its core are statements, which are instructions executed one after another, that produce side effects.
Statements don't produce values. On the other hand, expressions do. Expressions are:

- literal values (eg. numbers, strings, true, false),
- combinations of those (eg. two numbers added or multiplied, two booleans and'ed),
- assignments and
- function calls.

Everything else is a statement. `let` instructions don't evaluate to anything, as well as function and struct
declarations, and `if` and `for` instructions. Statements are:

- variable, function and struct declarations,
- if, while and for statements,
- return statements and
- blocks (scopes).

IL programs or scripts don't have an `entry point` function. The execution begins from the first statement down to the
last one, so function calls are allowed outside of functions.

A `Hello world` program in IL looks somewhat like this:

```txt
println("Hello, world!");
```

Semicolons after every statement is mandatory. It's a feature.

Blocks, also called `scopes`, are also allowed anywhere. They affect variables' scoping and lifetime. A variable
declared inside a block cannot be accessed from outside that block, but it can be accessed from other inner blocks.
Variables can be `shadowed` by other variables with the same name.

On the topic of variables, they are plain containers for any type of IL object. Variables are just `references` or
pointers to objects in memory. Objects are managed by a `reference-counting` system. As long as an object has a
reference somewhere, it stays alive. Care must be taken to not create circular references.

```txt
let x;  // Value is none
let number = 5;
let name = "Simon";
```

IL is a `dynamically-typed` language, which means variables don't have a specific type associated to them at
compile time and function parameter and return value types also don't have types. That spares the language for
the need of `generics`.

IL is also a `strongly-typed` language, because implicit convertions between types is not allowed. By design, you
can't add a float to an integer, or you can't evaluate a number as a boolean. The reason for this is that it
makes the language less error-prone, although more verbose.

```txt
let x = 5 - 3.7;  // This throws a runtime error
```

If statements and for and while loops look a lot like in the `JavaScript` programming language:

```txt
let age = 21.7;

if (age < 1.0) {
    println("Newborn");
} else if (age < 18.0) {
    println("Child");
} else {
    println("Adult");
}

for (let i = 0; i < 5; i = i + 1) {
    println("Iteration " + str(i));
}

let i = 5;

while (i > 0) {
    println("Still looping");
    i = i - 1;
}
```

Functions, as you might expect can take zero or more arguments and can return a meaningful value. If they don't
explicitly return something, none is returned. none is a special value that is like `null` in other languages.

```txt
identity(5);  // Error, identity is not defined yet

fun identity(x) {
    return x;
}

identity(none);  // Note that anything can be passed to functions
identity("something");

fun hello() {
    println("Hello");
}  // Implicitly returns none

hello();

fun factorial(n) {
    if (n < 2)  {
        return 1;
    }

    return n * factorial(n - 1);
}

factorial(5);
```

Structs are similar to `classes` in other languages, as they contain both fields and methods, but they don't
support `inheritance`, or `data hiding`. Structs are just a mutable bag of data. Attributes of an instance of
a struct can be created dynamically. Attributes inside structs are accessed by the special mandatory parameter
of methods that references the instance. That parameter is called *self* by convention. Structs don't support
`static` fields or methods.

```txt
struct Foo {}

let foo = Foo();  // Create an instance of the struct by calling its struct name

foo.some_data = 5;  // You might think already that I have an obsession with the number five
println(foo.some_data);

let foo2 = Foo();  // A brand new object
let foo3 = foo2;  // Not a brand new object

struct Data {
    init(self, a, b, c) {
        self.a = a;
        self.b = b;
        self.c = c;
        self.d = "hmm";
    }

    foo(self) {
        println(self.d);
    }

    bar(self, x) {
        self.a = self.a + x;
        self.foo();
    }
}

let data = Data(7, 3.14159, none);  // Booo, another five

data.bar(3);

```

You can see by now that IL also looks pretty similar to the `Python` programming language.

## Standard Library

The standard library of IL is very, very anemic. It consists of a few builtin functions:

- clock
- print
- println
- str
- int
- float
- bool
- input

print, println and input functions are the only ones for `IO`.

## What Is Missing Or What Could Be Added
