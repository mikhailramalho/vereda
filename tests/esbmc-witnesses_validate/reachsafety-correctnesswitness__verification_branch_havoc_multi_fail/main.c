extern int __VERIFIER_nondet_int(void);
int main() {
  int i = 0, s = __VERIFIER_nondet_int();
  while (i < 4) {
    s = s + 1;
    i++;
  }
  return s;
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-correctnesswitness/verification_branch_havoc_multi_fail
// Needs: --k-induction --validate-correctness-witness --witness witness.yml
// RUN: -m32 --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --k-induction --multi-property --validate-correctness-witness --witness witness.yml
// EXPECT: FAILED +\[main\.arithmetic-overflow\.2\] +line +5 +arithmetic overflow on add$
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
