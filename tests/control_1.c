//EXPECTED: 100

int main() {
    int i;
    int x;

    i = 0;
    x = 0;

    while (i < 10) {
        if (i == 5)
            x = 100;

        i = i + 1;
    }

    return x;
}
