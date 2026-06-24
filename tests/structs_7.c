//EXPECTED: 45

struct Vec2 {
    int x;
    int y;
};

int main() {
    struct Vec2 vec_arr[10];

    int i = 0;
    while ( i < 10 ) {
        vec_arr[i].x = i;
        vec_arr[i].y = i * 2;
        i = i + 1;
    }

    i = 0;
    int sum = 0;
    while ( i < 10 ) {
        sum = sum + vec_arr[i].x;
        i = i + 1;
    }

    return sum;
}
