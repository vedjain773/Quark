//EXPECTED: 99

int main() {
    int a[4];

    a[1 + 1] = 99;

    return a[2];
}
