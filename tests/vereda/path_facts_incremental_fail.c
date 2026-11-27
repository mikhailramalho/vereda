// RUN: --incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// x = y and the bounds of a value learnt on one side of a branch do not
// survive the join, nor reach the paths walked again from the loop's
// checkpoint at each larger bound (#296).
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int main(void) {
  int x = __VERIFIER_nondet_int(), y = __VERIFIER_nondet_int();
  int n = __VERIFIER_nondet_int();
  if (__VERIFIER_nondet_int()) {
    __VERIFIER_assume(x == y);
    __VERIFIER_assume(n < 2);
  }
  for (int i = 0; i < n && i < 3; i++)
    x += 0;
  __VERIFIER_assert(x == y || n >= 2);
  return 0;
}
