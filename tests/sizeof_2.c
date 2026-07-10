//EXPECTED: 16

int main() {
    int *p;
    char *cp;

    int total = sizeof(p) + sizeof(cp);

    return total;
}
