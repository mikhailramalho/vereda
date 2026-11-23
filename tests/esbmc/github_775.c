/* SV-COMP'22 c/memsafety/cmp-freed-ptr.c */

#include <stdlib.h>
#include <stdint.h>

int main() {

    struct T {
        struct T* next;
    };

    struct T* x = NULL;
    struct T* y = NULL;

    y = malloc(sizeof(*y));
    intptr_t adressY = (intptr_t) y;

    free(y);

    x = malloc(sizeof(*x));
    intptr_t adressX = (intptr_t) x;

    if (adressX == adressY)
    { // if the second malloc returns the same value as the first, I should get here
        free(x);
    }

    free(x);

    return 0;
}

// predator-regre/test-0238.c
// - comparing freed pointers
// - contributed by Ondra Lengal
// Imported from ESBMC regression/esbmc/github_775
// Needs: --state-hashing --add-symex-value-sets --k-step 2 --unlimited-k-steps --memory-leak-check REQUIRES arch32
// RUN: --no-div-by-zero-check --force-malloc-success --state-hashing --add-symex-value-sets --k-step 2 --floatbv --unlimited-k-steps --32 --memory-leak-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
