//EXPECTED: 0

struct Vec2 {
    int x;
    int y;
};

int dot(struct Vec2 c1, struct Vec2 c2) {
    return (c1.x * c2.x + c1.y * c2.y);
}

int main() {
    struct Vec2 v1;
    struct Vec2 v2;

    v1.x = 4;
    v1.y = 0;

    v2.x = 0;
    v2.y = 3;

    int dotProduct = dot(v1, v2);

    return dotProduct;
}
