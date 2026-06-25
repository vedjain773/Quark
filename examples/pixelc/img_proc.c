void grayscale(uint8_t* ptr, int size) {
    uint8_t grey;
    int r;
    int g;
    int b;

    int i = 0;
    while (i + 3 < size) {
        r = ptr[i];
        g = ptr[i + 1];
        b = ptr[i + 2];

        grey = (r + g + b) / 3;

        ptr[i] = grey;
        ptr[i + 1] = grey;
        ptr[i + 2] = grey;

        i = i + 4;
    }
}

void neg(uint8_t* ptr, int size) {
    int i = 0;
    while (i + 3 < size) {
        ptr[i] = 255 - ptr[i];
        ptr[i + 1] = 255 - ptr[i + 1];
        ptr[i + 2] = 255 - ptr[i + 2];

        i = i + 4;
    }
}
