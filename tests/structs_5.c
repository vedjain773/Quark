//EXPECTED: 5 

struct Complex {
    int real;
    int imag;
};

int main() {
    struct Complex c1;
    
    c1.real = 0;
    c1.imag = 2;

    struct Complex *c2 = &c1;

    c2->real = 5;
    c2->imag = 3;
    
    return c1.real;
}
