// This is ignored

println(2 + 2);

println(4 + 2 / 2);

println(90 * (1 - 3));

println(true);

println(false);

println(none);

let x;  // Value is none

println(x);

let name = "Simon";
println(name);
println("Hello, " + name + "!");

let x = 1;  // Shadows previous x
println(x);

x = 10;
println(x);

x = x + 1;
println(x);

let age = 21.7;

if (age < 1.0) {
    println("Newborn");
} else if (age < 18.0) {
    println("Child");
} else {
    println("Adult");
}
