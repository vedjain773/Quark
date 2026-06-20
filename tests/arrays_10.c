//EXPECTED: 1

int main() {
    int a[3];

    a[0] = 1;
    a[1] = 2;
    a[2] = 3;

    int* p;
    p = &a[1];

    return *(p - 1);
}
