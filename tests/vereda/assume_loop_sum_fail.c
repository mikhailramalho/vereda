// RUN: --unwind 9
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^\[INFO\] Walk time: .*\(1 check,
// assume_loop_sum with the bound one lower: every x at 10 reaches the
// error, so the bounds learned from the assumptions must not fold it.
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
  if (s > 10 * 8 - 1)
    reach_error();
  return 0;
}
