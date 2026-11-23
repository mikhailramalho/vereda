#include <assert.h>

int main() {
    int x = 30;
    float y = 0.1;

    assert(x == 30 && y >= 0.1 && x+y >= 30.1 );

    return 0;
}
// Imported from ESBMC regression/Interval-analysis-ibex-contractor/12-assert-success
// Needs: --interval-analysis --interval-analysis-ibex-contractor --k-induction --unlimited-k-step
// RUN: --interval-analysis --no-pointer-check --interval-analysis-ibex-contractor --k-induction --unlimited-k-step
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
