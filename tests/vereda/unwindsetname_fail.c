// unwindsetname.c's program with --unwind 10 but 5 for twice's second
// loop, which needs 6: its unwinding assertion fails.
// RUN: --no-summarize-array-loops --unwind 10 --unwindsetname twice:1:5
// EXPECT: FAILED +\[twice\.assertion\.1\] +line 14 +unwinding assertion loop 1$
// EXPECT: ^VERIFICATION FAILED$
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

static int twice(int n) {
  int a = 0, b = 0;
  for (int i = 0; i < n; i++)
    a++;
  for (int j = 0; j < 5; j++)
    b += 2;
  return a + b;
}

int main(void) {
  int n = __VERIFIER_nondet_int(), s = 0;
  __VERIFIER_assume(n >= 0 && n <= 1);
  for (int k = 0; k < 3; k++)
    s += twice(n);
  __VERIFIER_assert(s == 3 * (n + 10));
  return 0;
}
