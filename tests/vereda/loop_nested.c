// Nested loops with a break out of the inner one.
// RUN: --unwind 5
// RUN: --unwind 5 --incremental-solver
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);

int main(void) {
  int count = 0;
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++) {
      if (j > i)
        break;
      count++;
    }
  __VERIFIER_assert(count == 10);
  return 0;
}
