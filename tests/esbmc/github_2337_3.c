#include <assert.h>
#include <stdbool.h>

int main() {
    bool x = true;   // Equivalent to 1
    int result = -x; // Should be -1
    assert(result == -1);
    return 0;
}

// Imported from ESBMC regression/esbmc/github_2337_3
// RUN: --overflow-check
// EXPECT: ^VERIFICATION SUCCESSFUL$
