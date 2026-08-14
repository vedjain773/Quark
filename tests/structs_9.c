//EXPECTED: 7 

struct Point {
    int x;
    int y;
};

struct Point init() {
    struct Point p;
    p.x = 7;
    p.y = 0;
    return p;
}

int main() {
    return init().x; 
}
