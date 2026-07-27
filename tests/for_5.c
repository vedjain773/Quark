//EXPECTED: 6

int main() {
    int i;
    int j;
    int count = 0;

    for (i = 0; i < 3; i += 1) {
        for (j = 0; j < 2; j += 1) {
            count += 1;
        }
    }

    return count;
}
