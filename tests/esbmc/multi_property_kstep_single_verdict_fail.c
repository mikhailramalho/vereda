int __VERIFIER_nondet_int();

int main()
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assert(x != 1); // first
  __VERIFIER_assert(x != 2); // second
  return 0;
}
// Imported from ESBMC regression/esbmc/multi_property_kstep_single_verdict_fail
// RUN: --incremental-bmc --multi-property --max-k-step 5
// EXPECT: ^Properties: 2 checked, \d+ passed, 2 failed, \d+ not checked$
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \A(?![\s\S]*VERIFICATION FAILED[\s\S]*VERIFICATION FAILED)[\s\S]*\Z
