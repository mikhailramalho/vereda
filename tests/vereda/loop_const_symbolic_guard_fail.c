// Fail variant of loop_const_symbolic_guard.c: a bug inside a small
// constant-count loop under a satisfiable symbolic guard is still caught.
// RUN: --no-summarize-array-loops
// EXPECT: ^VERIFICATION FAILED$
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);

int main(void) {
  int a = __VERIFIER_nondet_int();
  int s = 0;
  if (a > 0) {
    for (int i = 0; i < 4; ++i)
      s += i;
    __VERIFIER_assert(s != 6);
  }
  return 0;
}
