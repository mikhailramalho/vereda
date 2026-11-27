#include <stdlib.h>

int main() {
    int *ptr = malloc(sizeof(int));
    int choice;

    if (choice == 1) {
        free(ptr);
    } else {
        *ptr = 42;        // potential NULL deref
    }

    return 0;
}
// Imported from ESBMC regression/esbmc/github_2629
// RUN: --multi-property --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
