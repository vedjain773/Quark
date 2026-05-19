//EXPECTED: 20

int main() {
    int x;
    int *p;

    x = 5;
    p = &x;

    *p = 20;

    return x;
}
