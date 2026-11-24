// Skipping the bounds a long loop of known trip count still needs doubles
// the bound at most, so a bug in its third iteration is found at bound 4
// without a walk of all million (#175).
// RUN: --no-summarize-array-loops --incremental-bmc --unlimited-k-steps
// EXPECT: Checking bound k = 4$
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
