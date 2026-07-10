//EXPECTED: 80

struct Point {
    int x;
    int y;
};

int main() {
    struct Point p[10];
    return sizeof(p);
}
