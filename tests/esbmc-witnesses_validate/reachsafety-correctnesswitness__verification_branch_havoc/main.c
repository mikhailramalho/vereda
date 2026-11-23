int main() {
  int i = 0, s = 0;
  while (i < 4) {
    s = s + 1;
    i++;
  }
  return s;
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-correctnesswitness/verification_branch_havoc
// Needs: --k-induction --validate-correctness-witness --witness witness.yml REQUIRES arch32
// RUN: --32 --overflow-check --k-induction --validate-correctness-witness --witness witness.yml
// MODE: FUTURE
// EXPECT: UNKNOWN +\[main\.arithmetic-overflow\.1\].*loop invariant too weak
// EXPECT: ^VERIFICATION UNKNOWN$
// Vereda: verdict none
