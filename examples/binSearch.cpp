#include <iostream>

extern "C" {
  int binSearch(int *int_p, int size, int target);
}

int main() {

  int arr[10] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19};

  int index = binSearch(arr, 10, 4);

  std::cout << "Found at index: " << index << "\n";

  return 0;
}
