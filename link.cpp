#include <iostream>

extern "C" {
    int fib(int n);
}

int main() {
    for (int i = 0; i < 10; i++) {
        std::cout << "Result: " << fib(i) << "\n";
    }
    return 0;
}
