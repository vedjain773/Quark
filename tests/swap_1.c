//EXPECTED: 10

void swap(int* a, int *b) {
  int c = *a;
  *a = *b;
  *b = c;
}

int main() {
  int x = 0;
  int y = 10;

  swap(&x, &y);

  return x;
}
