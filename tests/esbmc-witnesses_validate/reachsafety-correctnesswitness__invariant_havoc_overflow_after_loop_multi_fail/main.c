extern int __VERIFIER_nondet_int(void);
int main() {
  int i = 0, x = __VERIFIER_nondet_int();
  while (i < 4) {
    i++;
  }
  x = x + 1;
  return x;
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-correctnesswitness/invariant_havoc_overflow_after_loop_multi_fail
// Needs: --k-induction --validate-correctness-witness --witness witness.yml
// RUN: --overflow-check --k-induction --validate-correctness-witness --witness witness.yml --multi-property
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  FAILED .*line 7  arithmetic overflow on add$
// Vereda: verdict none
