#include <iostream>

extern "C" {
    char check();
}

int main() {

    std::cout << check() << "\n";

    return 0;
}
