//EXPECTED: 7

int main() {
    int x = 7;

    for (int i = 10; i < 5; i += 1) {
        x += 100;
    }

    return x;
}
