//EXPECTED: 15

int main() {
    int i;
    int sum;

    i = 1;
    sum = 0;

    while (i <= 5) {
        sum = sum + i;
        i = i + 1;
    }

    return sum;
}
