//EXPECTED: 5

int main() {
    int a[2][2];

    a[0][0] = 5;
    a[1][1] = a[0][0];

    return a[1][1];
}
