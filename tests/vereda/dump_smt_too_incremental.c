// Under --incremental-bmc, --dump-smt-too prints each bound's formula before
// solving it.
// RUN: --dump-smt-too --incremental-bmc
// EXPECT: Checking bound k = 1\n(.*\n)*\(check-sat\)(.*\n)*\[INFO\] Bound 1 too small
// EXPECT: Checking bound k = 3\n(.*\n)*\(check-sat\)(.*\n)*VERIFICATION SUCCESSFUL
extern void __VERIFIER_assert(int);
int main(void) {
  int s = 0;
  for (int i = 0; i < 2; i++)
    s += i;
  __VERIFIER_assert(s == 1);
  return 0;
}
