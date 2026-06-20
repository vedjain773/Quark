//EXPECTED: 2

int main() {
    int a[4];

    a[0] = 1;
    a[1] = 2;

    return a[a[0]];
}
