//EXPECTED: 16

struct Node {
    int value;
    struct Node *next;
};

int main() {
    return sizeof(struct Node);
}
