#include <assert.h>
int main() {
    __int128 X;
    __VERIFIER_assume(X>0);
    __int128 Z = 2;
    Z = Z << 70;
    assert((X+Z) > 0);
}
// Imported from ESBMC regression/esbmc/128_signed_false
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
