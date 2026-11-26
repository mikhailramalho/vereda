int __VERIFIER_nondet_int(void);
int main(void)
{
  int a = __VERIFIER_nondet_int();
  int b = __VERIFIER_nondet_int();
  return a << (b & 15);
}
// Imported from ESBMC regression/esbmc/github_9223_fail
// RUN: --overflow-check --solver z3 --std c11
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: arithmetic overflow on shl
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
