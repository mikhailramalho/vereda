// test_failure.c
extern void reach_error(void);

#define VALUE_MSB1 0x8000000000000000

typedef enum {
    VALUE = VALUE_MSB1
} enum_t;

enum_t foo() {
    return VALUE;
}

int main() {
    if (!(foo() == VALUE_MSB1)) {
        reach_error();
    }
    return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_1758
// EXPECT: ^VERIFICATION SUCCESSFUL$
