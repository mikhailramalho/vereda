#include <stdio.h>

typedef void *item_t[2];
typedef item_t *item_p;

int main(void) {
    /* malloc(2 * sizeof(void *)) */
    item_p item = malloc(sizeof *item);
    __VERIFIER_assume(item);

    // access within bounds
    (*item)[0] = (void *)0x1234;
    (*item)[1] = (void *)0xABCD;

    free(item);
    return 0;
}
// Imported from ESBMC regression/esbmc/github_2516
// EXPECT: ^VERIFICATION SUCCESSFUL$
