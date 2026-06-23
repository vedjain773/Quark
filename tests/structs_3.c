//EXPECTED: 1

struct Complex {
    int real;
    int imag;
};

int realAdd(struct Complex c1, struct Complex c2) {
    return (c1.real + c2.real);
}

int main() {
    struct Complex c1;
    struct Complex c2;

    c1.real = 0;
    c1.imag = 0;

    c2.real = 1;
    c2.imag = 2;

    int sum = realAdd(c1, c2);

    return sum;
}
