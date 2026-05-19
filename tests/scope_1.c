//EXPECTED: 1

int main() {
    int x;
    x = 1;

    {
        int x;
        x = 2;
    }

    return x;
}
