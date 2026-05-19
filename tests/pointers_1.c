//EXPECTED: 10

int main() {
    int x;
    int *p;

    x = 10;
    p = &x;

    return *p;
}
