//EXPECTED: 0 

struct Point {
    int x;
    int y;
};

struct Point init() {
    struct Point p;
    p.x = 0;
    p.y = 0;
    return p;
}

int getX(struct Point p) {
    return p.x; 
}

int main() {
    return getX(init()); 
}
