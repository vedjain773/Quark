//EXPECTED: 7

int main() {
    int a[2];

    a[0] = 7;

    return *(&a[0]);
}
