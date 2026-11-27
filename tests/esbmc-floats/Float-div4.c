#include <stdio.h>

void test_div_zero() {
    double x = 0.0;
    x += 1.0;        // x becomes 1.0
    x -= 1.0;        // x becomes 0.0 again
    x /= x;          // should fail
}

int main() {
    test_div_zero();
    return 0;
}
// Imported from ESBMC regression/floats/Float-div4
// Needs: --nan-check
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --nan-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
