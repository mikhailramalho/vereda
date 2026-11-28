// RUN: --unwind 31
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: ^\[INFO\] Walk time: .*\(0 checks,
// assume_loop_sum over 30 iterations: the check still folds, with nothing
// left for the solver.
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);
extern void reach_error(void);
int main(void) {
  int s = 0;
  for (int i = 0; i < 30; i++) {
    int x = __VERIFIER_nondet_int();
    __VERIFIER_assume(x >= 0 && x <= 10);
    s += x;
  }
  if (s > 10 * 30)
    reach_error();
  return 0;
}
