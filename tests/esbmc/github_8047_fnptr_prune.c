// #8047: every path parked at a function-pointer call records its domain, so
// x stays in [0, 10] across the merge and the claim is discharged by it.
int f(int y)
{
  return y + 1;
}

int g(int y)
{
  return y;
}

int main()
{
  int x, c;
  __VERIFIER_assume(x >= 0 && x <= 10);
  int (*fp)(int) = c ? f : g;
  fp(x);
  __VERIFIER_assert(x <= 10); // x at most 10
}
// Imported from ESBMC regression/esbmc/github_8047_fnptr_prune
// Needs: --interval-symex-assert
// RUN: --interval-symex-assert
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
