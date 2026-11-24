// A summarised loop still checks its body: one iteration at a nondet index
// stands for all of them, and this one fails at i == 77777.
// EXPECT: ^Summarised 1 array loop\(s\)$
// EXPECT: ^Violated property:\n  file \S*summary_check_fail\.c line 12 column 5 function main\n  __VERIFIER_assert\n  i != 77777$
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);
#define N 100000
int a[N];
int main(void) {
  for (int i = 0; i < N; i++) {
    a[i] = i;
    __VERIFIER_assert(i != 77777);
  }
  return 0;
}
