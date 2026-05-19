//EXPECTED: 1

int main() {
    int x;
    x = 1;

    {
        int x;
        x = 5;
        x = x + 1;
    }

    return x;
}
