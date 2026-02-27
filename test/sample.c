int fact(int n) {
    int num = n;
    int result = 1;

    while (num > 1) {
        result = result * n;
        n = n - 1;
    }

    return result;
}

int main() {
    int x = 5;
    int y = fact(x);

    if (y > 10) {
        y = y+1;
    } else {
        y = y-1;
    }

    return y;
}
