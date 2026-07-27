//EXPECTED: 13

int main() {
    int i;
    int x = 8;

    for (i = 0; i < 5; i += 1) {
        x += 1;
    }
    
    return x;
}
