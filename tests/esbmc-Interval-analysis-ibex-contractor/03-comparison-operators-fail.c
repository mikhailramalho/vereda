#include <assert.h>

int main() {
    int x = 50;
    float y = 0.1;

    if (x <= 1) {
        x = x + y;
        y = y + 1;
    } else if (x >= 20)
        x++;

    while (x > 45)
        x--;

    while (y < 1)
        y++;

    assert(x == y);

    return 0;
}
// Imported from ESBMC regression/Interval-analysis-ibex-contractor/03-comparison-operators-fail
// Needs: --interval-analysis --interval-analysis-ibex-contractor --k-induction
// RUN: --interval-analysis --ub-pointer-check=false --interval-analysis-ibex-contractor --k-induction --unlimited-k-steps
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
