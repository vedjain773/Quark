#include <iostream>

extern "C" {
    int check();
}

int main() {
    std::cout << check() << "\n";
    return 0;
}
