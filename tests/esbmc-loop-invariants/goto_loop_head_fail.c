// A goto to the head of a loop headed by an interval ASSUME starts a new
// iteration, so the copied body must check the invariant there.
int __VERIFIER_nondet_int();
int main() {
  int x = 0;
  __ESBMC_loop_invariant(x >= 0 && x <= 10);
L:
  while (x < 10) {
    x++;
    if (x == 5 && __VERIFIER_nondet_int()) {
      x = 50;
      goto L;
    }
  }
  __VERIFIER_assert(x <= 10); // x <= 10
  return 0;
}
// Imported from ESBMC regression/loop-invariants/goto_loop_head_fail
// Needs: --k-induction --loop-invariant --interval-analysis --interval-analysis-arithmetic
// RUN: --k-induction --loop-invariant --interval-analysis --interval-analysis-arithmetic --max-k-step 12
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
