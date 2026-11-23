extern int __VERIFIER_nondet_int(void);

// Non-singleton VLA size: the overflow2t guard's operand must NOT be collapsed
// to a constant, exercising the leaf-only optimisation path of the fix.
int main()
{
  int n = __VERIFIER_nondet_int();
  __VERIFIER_assume(n > 0 && n < 100);
  long packet[n];
  return 0;
}
// Imported from ESBMC regression/esbmc/github_5007_nondet
// Needs: --k-induction --interval-analysis
// RUN: --k-induction --interval-analysis --unwind 4
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
