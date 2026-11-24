// Under --incremental-bmc, while every path runs a loop of known trip count
// past the bound, the bounds below the one that unwinds it are skipped,
// doubling the bound at most: 1, 2, 4, ..., 512, then 1001 (#175).
// RUN: --no-summarize-array-loops --incremental-bmc --unlimited-k-steps
// EXPECT: Checking bound k = 512$
// EXPECT: Checking bound k = 1001$
// EXPECT-NOT: Checking bound k = 3$
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);
int main(void) {
  int i = 0;
  while (i < 1000)
    i++;
  __VERIFIER_assert(i == 1000);
  return 0;
}
