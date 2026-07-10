//EXPECTED: 16

int main() {
    int total = sizeof(int *) + sizeof(char *);

    return total;
}
