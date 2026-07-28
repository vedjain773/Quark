//EXPECTED: 6

int main() {
    int i = 0;
    int x = 0;

    for (; i < 8; i += 1) {
        if (i == 3)
            i += 2;
        x += 1;
    }

    return x;
}
