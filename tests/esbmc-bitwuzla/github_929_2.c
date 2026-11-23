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
// Imported from ESBMC regression/bitwuzla/github_929_2
// Needs: --output -
// RUN: --bitwuzla --smt-formula-only --output -
// MODE: FUTURE
// EXPECT: ^\(set-logic QF\_BV\)$
// Vereda: verdict none
