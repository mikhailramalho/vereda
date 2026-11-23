#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char name[10];

    // Accessing argv[1] without checking argc
    strcpy(name, argv[1]);  // Buffer overflow if input is too long!

    return 0;
}

// Imported from ESBMC regression/esbmc/github_2312_7
// RUN: --no-unwinding-assertions --unwind 1
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
