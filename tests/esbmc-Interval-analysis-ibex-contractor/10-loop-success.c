#include <assert.h>

int main() {
    int x = 30;
    float y = 0.1;

    for(int i=0; i < x/2;i++)
        x--;

    while (x > 40)
        x-=2;

    while (y < 10)
        y+=2.0;

    assert(x != 40 && y > 1);

    return 0;
}
// Imported from ESBMC regression/Interval-analysis-ibex-contractor/10-loop-success
// Needs: --interval-analysis --interval-analysis-ibex-contractor --k-induction
// RUN: --interval-analysis --no-pointer-check --interval-analysis-ibex-contractor --k-induction --unlimited-k-steps
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
