// A callee that may not return stops the loop part way, which a summary
// cannot express: the loop is unrolled, and the assertion after it is
// unreachable within the bound.
// RUN: --unwind 3 --no-unwinding-assertions
// EXPECT-NOT: Summarised
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);
int a[10];
void stop(void) {
  for (;;) {
  }
}
int main(void) {
  for (int i = 0; i < 10; i++) {
    if (i == 5)
      stop();
    a[i] = 1;
  }
  __VERIFIER_assert(a[9] == 2);
  return 0;
}
