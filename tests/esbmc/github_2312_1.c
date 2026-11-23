#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char name[20] = "";  // Ensure name starts empty

    if (argc > 1 && argv[1] != NULL)  // Check argc before accessing argv[1]
    {
        strncpy(name, argv[1], sizeof(name) - 1); // Copy safely, leaving space for null terminator
        name[sizeof(name) - 1] = '\0'; // Ensure null-termination
    }

    printf("Name: %s\n", name); // Print the copied name safely
    return 0;
}

// Imported from ESBMC regression/esbmc/github_2312_1
// RUN: --unwind 24
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
