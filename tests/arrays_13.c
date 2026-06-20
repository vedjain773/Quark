//EXPECTED: 1

int main() {
    int a[4];

    a[0] = 7;

    if (a[0])
        return 1;

    return 0;
}
