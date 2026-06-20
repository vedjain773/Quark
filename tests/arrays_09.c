//EXPECTED: 30

int main() {
    int a[3];

    a[0] = 10;
    a[1] = 20;
    a[2] = 30;

    int* p;
    p = &a[0];

    return *(p + 2);
}
