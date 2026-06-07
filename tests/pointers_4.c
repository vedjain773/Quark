//EXPECTED: 7

int main() {
    int x;
    int *p = &x;
    int **pp = &p;
    int ***ppp = &pp;

    ***ppp = 7;

    return x;
}
