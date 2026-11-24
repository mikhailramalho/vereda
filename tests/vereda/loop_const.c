// A constant loop needs no --unwind: unrolling stops when the solver finds
// the next iteration unreachable.
// RUN: --no-summarize-array-loops
// EXPECT: ^Unwinding loop 0 iteration 10 
// EXPECT-NOT: iteration 11
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);

int main(void) {
  int s = 0;
  for (int i = 0; i < 10; i++)
    s += 2;
  __VERIFIER_assert(s == 20);
  return 0;
}
