// Without --multi-property the path still ends at a recursive call cut at
// the bound, so the assertions after it see only the unwound runs.
// RUN: --unwind 2
// EXPECT: ^  FAILED       \[g\.assertion\.1\] .* recursion unwinding assertion$
// EXPECT: ^  SUCCESS      \[main\.assertion\.1\]
// EXPECT: ^  SUCCESS      \[main\.assertion\.2\]
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
