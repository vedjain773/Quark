//EXPECTED: 8

int main() {
  int x = 0;
  int *p = &x;

  int **a;
  a = &p;
  **a = 8;
  
  return x;
}
