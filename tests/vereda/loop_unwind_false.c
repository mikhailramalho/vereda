// The bound is too small: the unwinding assertion fails at the loop.
// RUN: --no-summarize-array-loops --unwind 5
// EXPECT: ^Violated property:\n  file \S*loop_unwind_false\.c line 10 column 3 function main\n  unwinding assertion loop 0$
// EXPECT: ^  FAILED       \[main\.assertion\.1\]  line 10  unwinding assertion loop 0$
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);

int main(void) {
  int s = 0;
  for (int i = 0; i < 10; i++)
    s += 2;
  __VERIFIER_assert(s == 20);
  return 0;
}
