//EXPECTED: 42

int main() {
    int a[2][2];

    int i;
    i = 1;

    a[i][0] = 42;

    return a[1][0];
}
