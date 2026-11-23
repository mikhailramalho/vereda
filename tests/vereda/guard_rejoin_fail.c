// A rejoined guard keeps every path into the join, and a condition the
// guard already has keeps its edge: after if/else diamonds fold back to
// their branch's guard, the callee tests a and !b again under a guard that
// has both, and the violation is still reached in the third iteration. The
// loop runs a nondet number of times, so the guards are real chains.
// RUN: --unwind 6 --no-unwinding-assertions
// EXPECT: ^VERIFICATION FAILED$
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
static void check(int a, int b, int k) {
  if (a)
    if (!b)
      __VERIFIER_assert(k != 2);
}
int main(void) {
  int k = 0;
  while (__VERIFIER_nondet_int()) {
    int a = __VERIFIER_nondet_int(), b = __VERIFIER_nondet_int(), x;
    if (a) {
      if (b)
        x = 1;
      else
        x = 2;
    } else {
      x = 3;
    }
    if (a)
      if (!b)
        check(a, b, k);
    k++;
  }
  return 0;
}
