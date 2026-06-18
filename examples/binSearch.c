int binSearch(int* int_p, int size, int target) {
  int left = 0;
  int right = size - 1;
  int mid = 0;

  while (left <= right) {
    mid = (left + right) / 2;
    int currEle = int_p[mid];

    if (currEle == target) {
      return mid;
    } else if (currEle > target) {
      right = mid - 1;
    } else if (currEle < target) {
      left = mid + 1;
    }
  }
    
  return -1;
}
