# IL lang

## Yet Another Programming Language

As of March 3, 2024, there are exactly 686 programming languages listed on the
[List of programming languages](https://en.wikipedia.org/wiki/List_of_programming_languages) article on Wikipedia.
Clearly we need more languages, as 686 is not enough.

IL lang is an imperative, structured, scripting, toy programming language made for learning purposes. *Scripting* is
maybe a little too much said, as IL doesn't offer all the necessary features needed for a full-fledged scripting
language. To be more exact, the core language features (data types, functions, structs, control flow) are somewhat
sufficient, but its standard library, builtin functions, is severely lacking, and it's also missing a foreign
function interface (FFI).

*IL* here stands for *Interpreted Language*. Although IL is *implemented* as an interpreted language, it could very
well actually be a compiled language to bytecode or even to machine code. *How* it's implemented has nothing to
do with the actual *language*.

This project is based on the second part of the amazing book [Crafting Interpreters](https://craftinginterpreters.com/)
by Robert Nystrom. IL is quite similar to the language created in the book, but it still ended up different
from it in some regards. I'll talk about the differences in a later section.

## Core Language Features And Syntax

### Expressions And Statements

IL features basic data types like integers, floats, booleans, strings, none (null type), but also aggregate types
(structs) and a few special ones. Because it is an imperative language, at its core there are statements,
instructions that are executed one after another, that produce side effects. Statements don't produce values.
On the other hand, expressions do. Expressions are:

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

### Hello World

IL programs or scripts don't have an `entry point` function. The execution begins from the first statement down to the
last one, so function calls are allowed outside of functions.

A `Hello World` program in IL looks somewhat like this:

```txt
println("Hello, world!");
```

Semicolons after every statement are mandatory. It's a feature.

### Blocks And Scoping

Blocks, also called `scopes`, are also allowed anywhere. They affect variables' scoping and lifetime. A variable
declared inside a block cannot be accessed from outside that block, but it can be accessed from other inner blocks.
Variables can be `shadowed` by other variables with the same name. At the top level, outside of functions and blocks
there is the global scope.

```txt
let var1 = "var1";

println(var1);  // Prints `var1`

{
    let var2 = "var2";
    let var1 = 5;

    println(var2);  // Prints `var2`
    println(var1);  // Prints `5`, the first var1 is shadowed

    {
        println(var2);  // Prints `var2`
    }
}

println(var2);  // Error, var2 is not defined
println(var1);  // Prints `var1`
```

### Variables

On the topic of variables, they are plain containers for any type of IL object. Variables are just `references` or
pointers to objects in memory. Objects are managed by a `reference-counting` system. As long as an object has a
reference somewhere, it stays alive. Care must be taken to not create circular references.

```txt
let x;  // Value is none

let number = 5;

let name = "Simon";
let name2 = name;  // References name from above

let value = "foo";
value = 5;
value = none;
```

IL is a `dynamically-typed` language, which means variables don't have a specific type associated to them at
declaration, in the source code, and function parameters and return values also don't have types. That spares
the language for the need of `generics`.

IL is also a `strongly-typed` language, because implicit convertions between types is not allowed. By design, you
can't add a float to an integer, or you can't evaluate a number as a boolean. The reason for this is that it
makes the language less error-prone, although more verbose.

```txt
let x = 5 - 3.7;  // This is a runtime error
```

### Control Flow

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

### Functions

Functions, as you might expect can take zero or more arguments and can return a meaningful value. If they don't
explicitly return something, none is returned. none is a special value that is like `null` in other languages.

```txt
identity(5);  // Error, identity is not defined yet

fun identity(x) {
    return x;
}

// Note that anything can be passed to the parameter
let nothing = identity(none);
let something = identity("something");

fun hello() {
    println("Hello");
}

hello();  // Implicitly returns none

fun factorial(n) {
    if (n < 2)  {
        return 1;
    }

    return n * factorial(n - 1);
}

factorial(5);
```

### Structs

Structs are similar to `classes` in other languages, as they contain both fields and methods, but they don't
support `inheritance`, or `data hiding`. Structs are just a mutable bag of data. Attributes of instances of
structs can be created dynamically. Attributes inside structs are accessed by a special mandatory parameter,
the first one, that references the instance. This parameter is called *self* by convention. Structs don't support
`static` fields or methods. *init* is a special method that is called automatically when an object is created.

```txt
struct Foo {}

let foo = Foo();  // Create an instance of the struct by calling its name

foo.some_data = 5;  // You might think already that I have an obsession with the number five, I don't
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

Both functions and structs can only be declared at the top level.

You can see by now that IL also looks pretty similar to the `Python` programming language.

## Standard Library

The standard library of IL is very, very anemic. It consists of just a few builtin functions:

- clock
- print
- println
- input
- str
- int
- float
- bool

print, println and input are the only functions that do `IO`.

## Keywords

This programming language has very few reserved words, only 14 in total, which should not be a surprise:

- let
- true
- false
- none
- or
- and
- not
- if
- else
- while
- for
- fun
- return
- struct

## Interpreter Itself

Besides executing scripts, IL's interpreter features a REPL (Read Evaluate Print Loop), which can be a quick and easy
way to execute some temporary code.

## What Is Missing Or What Could Be Added

IL, being just a hobby language, is not that big and it's very far from complete. There are lots of things that
could be added to improve the language and make it at least just a bit useful. Right now it's almost completely
useless, because the only input an IL program can receive is from the stdin file and the only output it can give
is through stdout. Some of the most important functionality that IL needs right now is:

- File and sockets IO,
- Command line arguments,
- Process return value,
- String operations,
- Arrays, maps, sets,
- Math functions,
- Break and continue statements,
- Introspection,
- Better error messages,
- Imports and
- FFI for bidirectional communication between IL and C++ code.

## Inner Workings And Implementation

### Execution Pipeline

When IL executes a script, it goes through a series of stages from reading the source code, understanding its
meaning and over to executing it.

#### Lexing

The first stage is called `lexing` or `scanning`, where it reads the source code character by character and converts it into
a series of `tokens`, each having a type. Besides that, they can also contain data.

For example, this statement:

```txt
let x = 2;
```

produces the following tokens:

```txt
Token::Let, Token::Identifier, Token::Equal, Token::Integer(2), Token::Semicolon
```

Note how the integer token contains the literal value.

In formal language theory, here, characters represent `symbols` and tokens represent `strings`. The `alphabet` that
IL uses, contains only a subset of the ASCII characters.

#### Parsing

The second stage, `parsing`, goes through all the tokens previously generated and tries to make some sense of
them by building an `abstract syntax tree` (AST). This tree represents the syntax of the language. Each node
of this tree represents a statement or an expression in the language and it contains data related to it.

There are many types of parsers, each having their strengths and weaknesses. IL, however, uses
`recursive descent parsing`, which is not too complex to implement, while also being quite robust. Recursive descent
becomes, as its name implies, a series of recursive functions used for iterating the tokens and building up
the tree.

A special document, called `lexical grammar`, formally describes the languages's syntax. The parser's code
structure is closely tied to this document. Usually each production in the grammar has a corresponding
function in the parser.

In this stage, tokens correspond to symbols and a series of tokens correspond to strings.

This abstract syntax tree can be used in a lot of ways. I'll talk about this shortly.

#### Analyzing

The third stage that IL runs in its execution is actually optional and it can be split in many, many smaller steps.

In this stage, the abstract syntax tree is traversed once or multiple times (depending on how many steps are there),
each time being `analyzed` for errors according to some conditions, for statically resolving identifiers, or many
other things. For example, here, statically-typed languages check their variable types. Or here, many languages detect
and report warnings or errors. IL uses this step to ensure that return, function and struct statements are placed
in valid positions.

IDEs use this tree to colorize the code, to implement code completion, or to give instant feedback without needing
to compile the code first.

#### Interpreting

Finally, the last stage is `interpreting` or executing the abstract syntax tree. This is the `runtime` part of
the language. Here, objects are created, variables, functions and structs are defined, things are calculated,
stuff is executed and runtime errors are thrown.

In either stage errors can appear. Thus, there are syntax and runtime errors.

### Visitor Pattern

For traversing the abstract syntax tree, I used the visitor pattern, which makes most of the project's code
more structured and organized. This technique lets dispatching a process function called `visit` for every
type of node in the tree by calling an `accept` function overriden in every node. In other words, it uses
both dynamic and static polymorphism to add a layer of indirection in order to better organize the code.
That is needed, because multiple classes (the analyzer, the interpreter etc.) need to process the tree.

### Object System

IL handles at runtime multiple objects: integers, floats, strings, booleans, functions, structs etc. In the
early versions, I implemented the base type `Object` as a union between all the actual types. That wasn't a
good idea, because those objects needed a lot to be passed and returned by value from functions, and each object
occupied an amount of memory equal to the largest type. Now, however, IL represents its objects as a hierarchy,
each object being dynamically-allocated. Passing and returning pointers doesn't pose a problem. But the disadvantage
is that memory is now more fragmented, it's less cache friendly and slower to access. Still, this change was good,
as shown in the following table:

|               | Adding floats X 1,000,000 | Concatenating strings X 100,000 |
|---------------|---------------------------|---------------------------------|
| First method  | 5.789 seconds             | 10.997 seconds                  |
| Second method | 4.697 seconds             | 3.407 seconds                   |

These tests were made with the `time` command on Linux. The interpreter was compiled in debug mode. It's
obviously not the most scientific or professional performnace test, but it still conveys the general idea that
dynamically-allocated objects are more appropriate in this language.

I solved the problem of memory management by using C++'s `shared_ptr` smart pointer, to automatically delete
unreachable objects.

For convenience, integers in IL are implemented as signed 64-bit integers, and floats as floating-points with
double precision, 64-bit as well.

Functions and structs are objects as well and they can be assigned to variables.

Structs are implemented as a `hash map` of strings (names) to objects (attributes). `Environments` are just hash
maps too, but they are chained, thus implementing scopes.

### Optimizations
