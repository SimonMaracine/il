fun factorial(n) {
    if (n < 2) {
        return 1;
    }

    return factorial(n - 1) * n;
}

fun gcd(a, b) {
    while (a != b) {
        if (a > b) {
            a = a - b;
        } else {
            b = b - a;
        }
    }

    return a;
}

let sum;

for (let i = 0; i < 1000; i = i + 1) {
    sum = 0;

    let i = 0;

    while (i < 100) {
        if (i > 50) {
            let result = gcd(i, i * 2);
            sum = sum + result * 100;
        } else {
            sum = sum + 1000 + sum / 2;
        }

        i = i + 1;
    }

    for (let i = 0; i < 15; i = i + 1) {
        let result = factorial(i);
        sum = sum - result * 12;
    }
}

println("Done " + str(sum));

