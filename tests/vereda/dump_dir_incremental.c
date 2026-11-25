// Under --incremental-bmc, each bound's formula gets its own file.
// RUN: --dump-smt-too --incremental-bmc --dump-dir out
// EXPECT: ^\[INFO\] Wrote out/formula-k1\.smt2$
// EXPECT: ^\[INFO\] Wrote out/formula-k3\.smt2$
// EXPECT: VERIFICATION SUCCESSFUL
extern void __VERIFIER_assert(int);
int main(void) {
  int s = 0;
  for (int i = 0; i < 2; i++)
    s += i;
  __VERIFIER_assert(s == 1);
  return 0;
}
