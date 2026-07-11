//EXPECTED: 48

int main() {
    int x = 7;
    int *p = &x;

    *p += 5;
    *p *= 2;

    return x + *p; 
}
