//EXPECTED: 15

int main() {
    int a[2];

    a[0] = 10;
    a[1] = a[0] + 5;

    return a[1];
}
