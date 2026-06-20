//EXPECTED: 123

int main() {
    int a[2][2][2];

    a[1][0][1] = 123;

    return a[1][0][1];
}
