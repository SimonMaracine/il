struct S {
    foo(self) {
        return 4;
    }
}

let s = S();

println(s.foo());
println(s.foo);

let m = s.foo;  // Taking a reference off a method makes it "forget" the instance
println(m(s));
println(m);

println(s.foo);
println(s.foo());
