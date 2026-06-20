//EXPECTED: 99

int main() {
    int a[2];

    a[0] = 10;

    a[a[0] - 10] = 99;

    return a[0];
}
