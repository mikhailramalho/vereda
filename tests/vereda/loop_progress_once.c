// Each loop prints an Unwinding loop line only the first time it reaches an
// iteration, and past 1024 only at powers of two (#383): the inner loop,
// walked three times, prints its iterations once, and the long loop prints
// 1024 and 2048 but none between them.
// RUN: --no-summarize-array-loops
// EXPECT: ^\[PROGRESS\] Unwinding loop 2 iteration 4\s
// EXPECT-NOT: (?s)(Unwinding loop 2 iteration \d+\s).*\1
// EXPECT: ^\[PROGRESS\] Unwinding loop 1 iteration 1024\s
// EXPECT-NOT: Unwinding loop 1 iteration 1025\s
// EXPECT: ^\[PROGRESS\] Unwinding loop 1 iteration 2048\s
// EXPECT-NOT: Unwinding loop 1 iteration 3000\s
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);
int main(void) {
  int s = 0;
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 4; j++)
      s += 1;
  for (int k = 0; k < 3000; k++)
    s += 2;
  __VERIFIER_assert(s == 6012);
  return 0;
}