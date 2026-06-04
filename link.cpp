#include <iostream>

extern "C" {
int arr(char *arr_p, int i);
}

int main() {

  char ar[5] = {0, 0, 0, 0, 0};

  for (int i = 0; i < 5; i++) {
    arr(&ar[4], i);
  }

  for (int i = 0; i < 5; i++) {
    std::cout << ar[i] << "\n";
  }

  return 0;
}
