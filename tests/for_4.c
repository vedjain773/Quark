//EXPECTED: 7

int main() {
    int i;
    int x = 7;

    for (i = 10; i < 5; i += 1) {
        x += 100;
    }

    return x;
}
