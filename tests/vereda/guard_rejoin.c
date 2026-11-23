// Guards rejoin and prune, as in SV-COMP's test_locks: each iteration sets
// a lock under p and later tests it under p again. The first diamond joins
// back to its branch's guard (g and p, or g and not p, is g), and under the
// second p the test lk != 1 is not p, which the guard already excludes, so
// the error's edge folds to false while the walk builds it and the check
// is false before the solver sees it. The loop runs a nondet number of
// times, so the guards are real chains, not true.
// RUN: --unwind 10 --no-unwinding-assertions --dump-smt-only
// EXPECT: ^\(assert false\)\n\(check-sat\)
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
int main(void) {
  while (__VERIFIER_nondet_int()) {
    int p1 = __VERIFIER_nondet_int(), p2 = __VERIFIER_nondet_int();
    int lk1 = 0, lk2 = 0;
    if (p1 != 0)
      lk1 = 1;
    if (p2 != 0)
      lk2 = 1;
    if (p1 != 0) {
      if (lk1 != 1)
        __VERIFIER_assert(0);
      lk1 = 0;
    }
    if (p2 != 0) {
      if (lk2 != 1)
        __VERIFIER_assert(0);
      lk2 = 0;
    }
  }
  return 0;
}
