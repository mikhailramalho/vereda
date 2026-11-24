// A goto to the first statement of a do-while skips its condition, so the
// copied body must check the invariant before the continue condition.
int __VERIFIER_nondet_int();
int main() {
  int x = 0;
  __ESBMC_loop_invariant(x >= 0 && x <= 10);
  do {
  top:
    x++;
    if (x == 5 && __VERIFIER_nondet_int()) { x = 7; goto top; }
  } while (x < 10);
  __VERIFIER_assert(x <= 10); // x <= 10
  return 0;
}
// Imported from ESBMC regression/loop-invariants/do_while_goto_head
// Needs: --loop-invariant
// RUN: --loop-invariant --max-k-step 12
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
