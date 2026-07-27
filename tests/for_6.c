//EXPECTED: 6

int main() {
    int i;
    int x = 0;

    for (i = 0; i < 8; i += 1) {
        if (i == 3)
            i += 2;
        x += 1;
    }

    return x;
}
