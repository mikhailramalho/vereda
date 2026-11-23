#include <assert.h>

int main()
{
  int a, b, c;

  a = __VERIFIER_nondet_int();
  b = __VERIFIER_nondet_int();
  c = a + b;
  assert(c == a + b);

  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_929_4
// Needs: --output -
// RUN: --smt-formula-only --output -
// MODE: FUTURE
// EXPECT: ^\(check-sat\)$
// Vereda: verdict none
