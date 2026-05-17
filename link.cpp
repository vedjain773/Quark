#include <iostream>

extern "C" {
    int swap(int* a, int* b);
}

int main() {

    int a = 5;
    int b = 10;

    swap(&a, &b);

    std::cout << "A: " << a << "\nB: " << b << "\n";

    return 0;
}
