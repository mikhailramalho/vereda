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
// Imported from ESBMC regression/esbmc/github_24
// RUN: --no-unwinding-assertions --unwind 1
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
