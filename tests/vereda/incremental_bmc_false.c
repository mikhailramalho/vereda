// --incremental-bmc raises the bound past the failed unwinding assertions
// until the bug at the third iteration is reachable.
// RUN: --no-summarize-array-loops --incremental-bmc
// EXPECT: ^\[INFO\] Bound 1 too small: unwinding assertion loop 0$
// EXPECT: ^\[INFO\] Bound 2 too small: unwinding assertion loop 0$
// EXPECT-NOT: ^\[INFO\] Bound 3 too small
// EXPECT: ^Violated property:\n  file \S*incremental_bmc_false\.c line 15 column 5 function main\n  __VERIFIER_assert\n  i != 2$
// EXPECT: ^VERIFICATION FAILED$
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);

int main(void) {
  int n = __VERIFIER_nondet_int();
  for (int i = 0; i < n; ++i)
    __VERIFIER_assert(i != 2);
  return 0;
}
