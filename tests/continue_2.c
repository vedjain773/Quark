//EXPECTED: 0 

int main() {
    int i = 0;
    int sum = 0;
    while (i <= 10) {
        i = i + 1;
        
        continue;

        sum = sum + 1;
    }
    
    return sum;
}
