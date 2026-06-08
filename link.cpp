#include <iostream>

extern "C" {
  int sort(int *int_p, int size);
}

int main() {

  int arr[10] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};

  sort(arr, 10);

  for (int i = 0; i < 10; i++) {
    std::cout << arr[i] << "\n";
  }

  return 0;
}
