// A recursion unwinding assertion is an assertion, numbered with the
// function's other assertions by position, as in ESBMC 8.8.
// RUN: --unwind 3 --multi-property
// EXPECT: ^  FAILED       \[f\.assertion\.1\]  line 13  __VERIFIER_assert$
// EXPECT: ^  FAILED       \[f\.assertion\.2\]  line 16  recursion unwinding assertion$
// EXPECT: ^  FAILED       \[main\.assertion\.1\]  line 20  __VERIFIER_assert$
// EXPECT-NOT: recursion\.
// EXPECT: ^VERIFICATION FAILED$
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);

int f(int n) {
  __VERIFIER_assert(n != 100);
  if (n <= 0)
    return 0;
  return 1 + f(n - 1);
}
int main(void) {
  int n = __VERIFIER_nondet_int();
  __VERIFIER_assert(n != 5);
  return f(n);
}
