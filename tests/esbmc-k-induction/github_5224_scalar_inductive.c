// Issue #5224 negative control: a loop that modifies only a scalar (no
// pointer write) must NOT trip the pointer-write gate, so the inductive
// step stays enabled and proves this unbounded-in-the-base-case program.
// Matching "Solution found by the inductive step" confirms the gate did
// not fire (a spurious gate would force VERIFICATION UNKNOWN instead).
extern int __VERIFIER_nondet_int(void);

int main(void)
{
  int n = __VERIFIER_nondet_int();
  __VERIFIER_assume(n > 0);

  int x = 0;
  for (int i = 0; i < n; i++)
    x = 0;

  __VERIFIER_assert(x == 0); // x stays zero across all iterations
  return 0;
}
// Imported from ESBMC regression/k-induction/github_5224_scalar_inductive
// Needs: --k-induction --max-inductive-step 5
// RUN: --k-induction --max-inductive-step 5
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: Solution found by the inductive step
// Vereda: verdict none
