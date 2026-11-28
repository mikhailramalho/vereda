// --dump-dir writes the dumps to files instead of printing them.
// RUN: --dump-ir-too --dump-smt-too --dump-dir out --no-incremental-bmc
// EXPECT: ^\[INFO\] Wrote out/ir\.ll$
// EXPECT: ^\[INFO\] Wrote out/formula\.smt2$
// EXPECT: VERIFICATION SUCCESSFUL
// EXPECT-NOT: ^define|^\(assert
extern void __VERIFIER_assert(int);
int main(void) {
  int x = 3;
  __VERIFIER_assert(x == 3);
  return 0;
}
