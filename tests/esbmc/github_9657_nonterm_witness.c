// Issue #9657: a terminating loop yields no non-termination witness.
extern int __VERIFIER_nondet_int(void);
int main(void)
{
  int x = __VERIFIER_nondet_int();
  while (x > 0)
    x--;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9657_nonterm_witness
// Needs: --termination --max-inductive-step 3 --k-step 2 --unlimited-k-steps --witness-output-yaml -
// RUN: --termination --max-inductive-step 3 --k-step 2 --unlimited-k-steps --no-pointer-check --no-bounds-check --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: \A(?![\s\S]*action: "cycle")[\s\S]*\Z
// Vereda: verdict none
