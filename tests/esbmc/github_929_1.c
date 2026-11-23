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
// Imported from ESBMC regression/esbmc/github_929_1
// Needs: --output -
// RUN: --z3 --smt-formula-too --output -
// MODE: FUTURE
// EXPECT: ^\(set-option :produce-models true\)$
// Vereda: verdict none
