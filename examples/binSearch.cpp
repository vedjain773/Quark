#include <iostream>

extern "C" {
  int binSearch(int *int_p, int size, int target);
}

int main() {

  int arr[10] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19};

  int index[4];

  index[0] = binSearch(arr, 10, 9);
  index[1] = binSearch(arr, 10, 1);
  index[2] = binSearch(arr, 10, 19);
  index[3] = binSearch(arr, 10, 4);

  for (int i = 0; i < 4; i++) {
    std::cout << index[i] << "\n";
  } 

  return 0;
}
