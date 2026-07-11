//EXPECTED: 24 

int main() {
    int a[3];

    a[0] = 5;
    a[1] = 8;

    a[0] += 3;
    a[1] *= 2;

    return a[0] + a[1];
}
