#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // No check for argc before using argv[1]
    int size = atoi(argv[1]);

    // No check if size is reasonable before malloc
    int *arr = malloc(size * sizeof(int));

    // Dereferencing NULL if malloc fails
    arr[0] = 42;

    free(arr);
    return 0;
}

// Imported from ESBMC regression/esbmc/github_2312_8
// RUN: --no-unwinding-assertions --unwind 1
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
