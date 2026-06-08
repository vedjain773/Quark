void swap(int* a, int* b) {
  int c = *a;
  *a = *b;
  *b = c;
}

int isSorted(int* int_p, int size) {
  int i = 0;
  int curr = 0;
  int next = 0;
  while (i < size - 1) {
    curr = *(int_p + i);
    next = *(int_p + i + 1);
    
    if (curr > next)
      return 0;

    i = i + 1;
  }

  return 1;
}

int sort(int* int_p, int size) {
  int i = 0;
  int curr = 0;
  int next = 0;

  while (isSorted(int_p, size) != 1) {
    i = 0;
    while (i < size - 1) {
      curr = *(int_p + i);
      next = *(int_p + i + 1);

      if (curr > next)
        swap((int_p + i), (int_p + i + 1));

      i = i + 1;
    }
  }
    
  return 0;
}
