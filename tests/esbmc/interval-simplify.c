int main()
{
 int a,*q;
 a = 1;
 q = &a;
 *q = 2;
 __VERIFIER_assert(a != 2);
}
// Imported from ESBMC regression/esbmc/interval-simplify
// Needs: --interval-analysis --interval-analysis-simplify
// RUN: --interval-analysis --interval-analysis-simplify
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
