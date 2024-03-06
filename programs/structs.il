// Methods must have an instance; by convention it is self

struct S1 {}

struct S2 {
    foo(self) {
        return 4;
    }

    bar(self, something) {
        println("bar " + something);
    }
}

struct S3 {
    // foo() {}
}

// -----------------------------

let s1 = S1();

println(S1);
println(s1);

let s2 = S2();

println(s2.foo());
s2.bar("something");
println(s2.bar);
