//EXPECTED: 11

int main() {
    struct Point {
        int coords[2];
    };

    struct Point p1;

    p1.coords[0] = 5;
    p1.coords[1] = 6;

    int sum = p1.coords[0] + p1.coords[1];

    return sum;
}
