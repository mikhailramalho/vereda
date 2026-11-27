#include <stdlib.h>

int main() {
    int *ptr = (int*) malloc(sizeof(int));
    *ptr = 42;
    free(ptr);
    *ptr = 50;

    return 0;
}

// Imported from ESBMC regression/esbmc/dangling_assume
// Needs: --conv-assert-to-assume
// RUN: --force-malloc-success --conv-assert-to-assume --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
