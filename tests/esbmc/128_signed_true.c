#include <assert.h>
int main() {
    __int128 Z = 2;
    Z = Z << 70;
    assert(Z > 0);
}
// Imported from ESBMC regression/esbmc/128_signed_true
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
