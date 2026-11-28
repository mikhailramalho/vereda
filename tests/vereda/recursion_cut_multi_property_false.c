// Under --multi-property a path goes on past a recursive call cut at the
// bound, as in ESBMC: g(2) is cut at its third call, its result is free, so
// both later assertions fail on that path.
// RUN: --unwind 2 --multi-property
// EXPECT: ^  FAILED       \[g\.assertion\.1\] .* recursion unwinding assertion$
// EXPECT: ^  FAILED       \[main\.assertion\.1\]
// EXPECT: ^  FAILED       \[main\.assertion\.2\]
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);

int g(int d) {
  if (d == 0)
    return 0;
  return g(d - 1) + 1;
}
int main(void) {
  int n = __VERIFIER_nondet_int();
  if (n < 1 || n > 2)
    return 0;
  int r = g(n);
  __VERIFIER_assert(r == n);
  __VERIFIER_assert(n != 2);
  return 0;
}
