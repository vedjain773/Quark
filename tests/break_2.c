//EXPECTED: 5

int main() {
    int i;
    for (i = 0; i <= 10; i += 1) {
        if (i == 5)
            break;
    }
    
    return i;
}
