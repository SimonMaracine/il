// Return statements outside of functions are invalid
// Function declarations are invalid inside of other function declarations

// return 5;

fun outer() {
    let foo = 3;

    // fun inner() {
    //     println(foo);
    // }

    // inner();
}

outer();
