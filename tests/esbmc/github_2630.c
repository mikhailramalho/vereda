#include <stdlib.h>

int main() {
    int *resource1 = malloc(sizeof(int));
    int *resource2 = malloc(sizeof(int));
    int choice;  // symbolic input

    // Multiple resource management paths
    if (choice == 1) {
        free(resource1);
        free(resource2);
    } else if (choice == 2) {
        free(resource1);
    } else {
        *resource1 = 42;
        free(resource1);
        *resource1 = 99;  // use-after-free
    }

    return 0;
}
// Imported from ESBMC regression/esbmc/github_2630
// RUN: --multi-property
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
