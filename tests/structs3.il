struct Human {
    init(self, name, age, height) {
        self.name = name;
        self.age = age;
        self.height = height;
    }

    say_hi(self) {
        println("Hello! My name is " + self.name + ".");
    }

    is_adult(self) {
        return self.age >= 18;
    }

    get_height(self) {
        return self.height;
    }

    set_height(self, height) {
        self.height = height;
    }
}

let simon = Human("Simon", 21, 1.7);
simon.say_hi();
println(simon.is_adult());
println(simon.get_height());
simon.set_height(1.8);
println(simon.height);

let alice = Human("Alice", 13, 1.5);
alice.say_hi();
println(alice.is_adult());
println(alice.get_height());

simon.say_hi();
println(simon.is_adult());

alice.say_hi();
println(alice.is_adult());
