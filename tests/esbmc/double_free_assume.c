#include <stdlib.h>

int main() {
    int *ptr = (int*) malloc(sizeof(int));
    *ptr = 42;
    free(ptr);
    free(ptr);

    return 0;
}

// Imported from ESBMC regression/esbmc/double_free_assume
// Needs: --conv-assert-to-assume
// RUN: --force-malloc-success --conv-assert-to-assume --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
