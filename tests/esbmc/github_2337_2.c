#include <limits.h>

int main() {
    int a = INT_MIN;  // Minimum value for int
    int b = 0;        // Any value greater than INT_MIN
    int result = -a;  // Overflow occurs if `a` is INT_MIN
    return 0;
}
// Imported from ESBMC regression/esbmc/github_2337_2
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
