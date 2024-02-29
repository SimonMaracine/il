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

hello();

say_hi("Simon");

// say_hi("what", 4, -9);

add(1, 4);
add("Hello, ", "world!");
