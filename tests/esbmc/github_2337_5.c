#include <stdbool.h>
#include <limits.h>

int main() {
    bool x = true;
    int max_val = INT_MAX;
    int result = -(max_val + x); // Should detect overflow
    return 0;
}

// Imported from ESBMC regression/esbmc/github_2337_5
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check
// EXPECT: ^VERIFICATION FAILED$
