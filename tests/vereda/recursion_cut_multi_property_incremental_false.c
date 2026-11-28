// Under --incremental-bmc a cut call waits for a larger bound instead of
// going on with a free result, so only the second assertion fails.
// RUN: --incremental-bmc --multi-property
// EXPECT: ^  SUCCESS      \[main\.assertion\.1\]
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
