// --unwindsetname gives loop idx of function name its own bound in place
// of --unwind (#310), here more than the 2 the other loops get: twice's
// second loop needs 6, main's loop 4. Loops number from 0 in each
// function, and a name may be clang's USR. In the second run only twice's
// first loop gets 2, which it needs, and every other loop 6. Summaries are
// off so that every loop unrolls.
// RUN: --no-summarize-array-loops --unwind 2 --unwindsetname twice:1:6,c:@F@main#:0:4
// RUN: --no-summarize-array-loops --unwind 6 --unwindsetname twice:0:2
// EXPECT: ^VERIFICATION SUCCESSFUL$
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
