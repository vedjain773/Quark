//EXPECTED: 3

int main() {
    int i;

    for (i = 0; ; i += 1) {
        if (i == 3)
            break;
    }

    return i;
}
