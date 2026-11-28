// RUN: --unwind 9
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: ^\[INFO\] Walk time: .*\(0 checks,
// A sum of nondet ints, each bounded by an assumption of a conjunction
// inside the loop, is at most 10 per iteration, which the path's bounds
// show as the check is built, so it folds (#453). The && joins two paths,
// and the phi there must not repeat the path's guard, or learning the
// assumption spends its budget on the earlier iterations' conditions.
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);
extern void reach_error(void);
int main(void) {
  int s = 0;
  for (int i = 0; i < 8; i++) {
    int x = __VERIFIER_nondet_int();
    __VERIFIER_assume(x >= 0 && x <= 10);
    s += x;
  }
  if (s > 10 * 8)
    reach_error();
  return 0;
}
