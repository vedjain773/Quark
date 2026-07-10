//EXPECTED: 5

int main() {
    int a;
    char b;

    int total = sizeof(a) + sizeof(b);

    return total;
}
