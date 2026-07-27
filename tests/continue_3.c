//EXPECTED: 0

int main() {
    int i;
    int sum = 0; 
   
    for (i = 0; i <= 10; i += 1) {
        continue;
        sum = sum + 1;
    }

    return sum;
}
