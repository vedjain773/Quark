//EXPECTED: 2

int main() {
    int x = 4;
    int y;

    if (x == 0) {
        y = 0;
    } else if (x == 2) {
        y = 1;
    } else {
        y = 2;
    }

    return y;
}
