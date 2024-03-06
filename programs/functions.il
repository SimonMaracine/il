fun hello() {
    println("Hello");
}

fun say_hi(name) {
    println("Hi, " + name + "!");
}

fun add(a, b) {
    println(a + b);
}

println(hello);
println(say_hi);
println(add);

let result = hello();
println(result);

say_hi("Simon");

// say_hi("what", 4, -9);

add(1, 4);
add("Hello, ", "world!");

// ---------------------------------

fun factorial(n) {
    if (n < 2) {
        return 1;
    }

    return n * factorial(n - 1);
}

println(factorial(6));

fun return_early() {
    println("This is printed");

    return;

    println("This is not printed");
}

return_early();

fun random_number() {
    return 98;
}

println(random_number());
