// Assertion continuation guards make this loop conditional, preventing the
// unconditional-prefix bound jump (#175). Its third-iteration bug is found
// at bound 3 without a walk of all million iterations.
// RUN: --no-summarize-array-loops --incremental-bmc --unlimited-k-steps
// EXPECT: Checking bound k = 3$
// EXPECT-NOT: Checking bound k = 8$
// EXPECT: ^VERIFICATION FAILED$
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
int main(void) {
  int s = 0;
  for (int i = 0; i < 1000000; i++) {
    s += __VERIFIER_nondet_int() & 1;
    __VERIFIER_assert(s < 3);
  }
  return 0;
}
