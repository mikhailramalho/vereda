#include <assert.h>
int main() {
    int values[5];
    assert(values[1] == 10);
}

// Imported from ESBMC regression/esbmc/github_2182
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
