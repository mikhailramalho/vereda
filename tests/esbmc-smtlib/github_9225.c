/* github #9225: subtracting pointers to elements of size 1 or 2^k needs no
 * divider. */
unsigned __VERIFIER_nondet_uint(void);
int a[10];
char c[10];

int main(void)
{
  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 10);
  __VERIFIER_assert(&a[i] - a + (&c[i] - c) == 2 * i);
  return 0;
}
// Imported from ESBMC regression/smtlib/github_9225
// Needs: --smtlib --output out.smt2 CHECK_FILE out.smt2 contains bvashr CHECK_FILE out.smt2 absent bvsdiv
// RUN: --smtlib --smt-formula-only --output out.smt2 --64
// MODE: FUTURE
// EXPECT: ^SMT formula written to output file out\.smt2$
// Vereda: verdict none
