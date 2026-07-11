//EXPECTED: 0

int main() {
    int x = 5;
    int y;

    y = (x += 3);

    return x - y;
} 
