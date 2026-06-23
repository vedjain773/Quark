//EXPECTED: 8

int main() {
    struct Point {
        int x;
        int y;
    };

    struct Point p1;

    p1.x = 5;
    p1.y = 3;

    return p1.x + p1.y;
}
