unsigned __VERIFIER_nondet_uint(void);
int main(void) {
  unsigned x = __VERIFIER_nondet_uint();
  __VERIFIER_assert(x == x); // refl
  return 0;
}
// Imported from ESBMC regression/goto2lean/output_file
// Needs: --goto2lean --output out.lean SEED_FILE out.lean stale proof CHECK_FILE out.lean contains ^theorem main_safe : Prog\.main \(fun _ => True\) := by$ CHECK_FILE out.lean contains ^  \(x = x\) ∧ \($ CHECK_FILE out.lean absent stale proof
// RUN: --goto2lean --output out.lean
// MODE: FUTURE
// EXPECT: ^GOTO program creation time
// Vereda: verdict none
