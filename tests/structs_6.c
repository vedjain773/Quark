//EXPECTED: 8

struct Node {
    int value;
    struct Node *next;
};

int main() {
    struct Node a;
    struct Node b;
    struct Node c;

    a.next = &b;
    b.next = &c;

    a.next->next->value = 8;

    return c.value;
}
