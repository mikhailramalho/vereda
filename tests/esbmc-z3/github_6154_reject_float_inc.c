// GitHub #6154: `++' on a non-integer local is NOT summarizable -- the
// summary builds `cur + from_integer(1, t)', which is nil for a float, so
// accepting this crashed once the summary reached the solver.  The callee must
// be refused, leaving the loop to be unwound normally.
int f(int n)
{
  double d = 0.0;
  for (int i = 0; i < 3; i++)
    d++;
  return n + (int)d;
}

int main()
{
  int k;
  __VERIFIER_assert(__ESBMC_forall(&k, f(k) == k + 3)); // float inc not summarized
  return 0;
}
// Imported from ESBMC regression/z3/github_6154_reject_float_inc
// Needs: __ESBMC_forall
// RUN: --solver z3
// MODE: FUTURE
// EXPECT: ^\[PROGRESS\] Unwinding loop .* function f$
// EXPECT: ^VERIFICATION SUCCESSFUL$
