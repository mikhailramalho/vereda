// RUN: --incremental-bmc --k-step 2 --unlimited-k-steps
// EXPECT: Checking bound k = 1001$
// EXPECT-NOT: Checking bound k = 999$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// A bounded loop with a symbolic early exit (`y < 1000 && nondet()`) exposes
// its constant max backedge-taken count to SCEV after trivial i1-phi threading,
// so incremental BMC doubles bounds instead of stepping linearly (#253).
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
int main(void) {
  int x = 1, y = 0;
  while (y < 1000 && __VERIFIER_nondet_int()) {
    x = x + y;
    y = y + 1;
  }
  __VERIFIER_assert(x >= y);
  return 0;
}
