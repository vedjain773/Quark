//EXPECTED: 20

int main() {
    int i;
    int sum = 0;

    for (i = 0; i < 10; i += 2) {
        sum += i;
    }

    return sum;
}
