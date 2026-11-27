#include <assert.h>
#include <stdbool.h>

int main() {
    bool x = false;  // Equivalent to 0
    int result = -x; // Should be 0
    assert(!x);
    return 0;
}

// Imported from ESBMC regression/esbmc/github_2337_4
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
