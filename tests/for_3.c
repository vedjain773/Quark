//EXPECTED: 15

int main() {
    int i;
    int x = 0;

    for (i = 5; i > 0; i -= 1) {
        x += 3;
    }

    return x;
}
