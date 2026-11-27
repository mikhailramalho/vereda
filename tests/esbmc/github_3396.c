#include <assert.h>

int __VERIFIER_nondet_int(void);
void __VERIFIER_assume(_Bool);

typedef int (*handler_t)(int);

int add_one(int x) { return x + 1; }
int sub_one(int x) { return x - 1; }

handler_t handlers[2] = { add_one, sub_one };

int main() {
    int idx = __VERIFIER_nondet_int();
    __VERIFIER_assume(idx >= 0 && idx < 2);

    handler_t h = handlers[idx];
    int result = h(5);

    assert(result == 6 || result == 4);
    return 0;
}
// Imported from ESBMC regression/esbmc/github_3396
// EXPECT: ^VERIFICATION SUCCESSFUL$
