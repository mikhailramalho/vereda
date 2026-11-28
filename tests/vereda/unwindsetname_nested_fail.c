// unwindsetname_nested.c's program with 2 for loop 0, the inner loop, which
// needs 3: its unwinding assertion, on line 11, fails.
// RUN: --no-summarize-array-loops --unwind 10 --unwindsetname main:0:2
// EXPECT: FAILED +\[main\.assertion\.1\] +line 11 +unwinding assertion loop 0$
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);

int main(void) {
  int s = 0;
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 2; j++)
      s++;
  for (int k = 0; k < 2; k++)
    s++;
  __VERIFIER_assert(s == 8);
  return 0;
}
