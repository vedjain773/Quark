//EXPECTED: 70

int main() {
    int x;
    int y;
    int *p;

    x = 10;
    y = 20;

    p = &x;
    *p = 50;

    return x + y;
}
