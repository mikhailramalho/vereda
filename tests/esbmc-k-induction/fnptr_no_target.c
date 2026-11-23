extern int __VERIFIER_nondet_int(void);

int main(void)
{
  void (*f)(void) = 0;
  if (__VERIFIER_nondet_int())
    f();

  int n = __VERIFIER_nondet_int();
  __VERIFIER_assume(n > 0);

  int x = 0;
  for (int i = 0; i < n; i++)
    x = 0;

  __VERIFIER_assert(x == 0); // x stays zero across all iterations
  return 0;
}
// Imported from ESBMC regression/k-induction/fnptr_no_target
// Needs: --k-induction --max-inductive-step 5
// RUN: --k-induction --max-inductive-step 5 --no-pointer-check --no-bounds-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: Solution found by the inductive step
// Vereda: verdict none
