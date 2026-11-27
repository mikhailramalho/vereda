#include <assert.h>

int main() {
    int x = 50;
    float y = 0.1;

    if (x < 1 && y < 1) {
        x = x + y;
        y = y + 1;
    } else if (x >= 20 || x < 0)
        x++;

    while (x > 40 && y > 1)
        x--;

    while (y < 1 || x < 3)
        y++;

    assert(x != y && x != 0 || y == 0);

    return 0;
}
// Imported from ESBMC regression/Interval-analysis-ibex-contractor/06-logic-operators-success
// Needs: --interval-analysis --interval-analysis-ibex-contractor --k-induction
// RUN: --interval-analysis --ub-pointer-check=false --interval-analysis-ibex-contractor --k-induction --unlimited-k-steps
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
