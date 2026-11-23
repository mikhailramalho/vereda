// #8047: an early return parks a path at the end of the function; its domain
// must be joined back there too.
int g;
int __VERIFIER_nondet_int();

void f()
{
  if (g == -128)
    return;
  __VERIFIER_assume(g == 7);
}

int main()
{
  g = __VERIFIER_nondet_int();
  f();
  __VERIFIER_assert(g > -7); // g above -7
}
// Imported from ESBMC regression/esbmc/github_8047_return
// Needs: --interval-symex-assert
// RUN: --interval-symex-assert
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
