//EXPECTED: 8

struct Point {
    int x;
    int y;
};

int main() {
    struct Point p;
    return sizeof(p);
}
