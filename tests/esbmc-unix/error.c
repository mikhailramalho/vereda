#define _GNU_SOURCE
#include <error.h>
#include <assert.h>
#include <string.h>

int main(void) {
    // This should just print to stderr, not exit
    error(0, 0, "This is a test error (non-fatal)");

    // Program continues → assertion should pass
    assert(1 == 1);

    return 0;
}
// Imported from ESBMC regression/esbmc-unix/error
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
