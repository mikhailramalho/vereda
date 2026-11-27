// --no-summarize-array-loops unrolls the loops a summary would replace.
// RUN: --no-summarize-array-loops --no-incremental-bmc
// EXPECT-NOT: Summarised
// EXPECT: ^\[PROGRESS\] Unwinding loop 0 iteration 10
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);
int a[10];
int main(void) {
  for (int i = 0; i < 10; i++)
    a[i] = i;
  __VERIFIER_assert(a[4] == 4);
  return 0;
}
