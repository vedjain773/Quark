//EXPECTED: 55

int main() {
    int a[2];

    a[0] = 55;

    int* p;
    p = &a[0];

    return *p;
}
