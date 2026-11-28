// Past a recursive call cut at the bound, the callee's effects are skipped,
// as in ESBMC: f(2) is cut at f(0), so x keeps the 1 that f(1) stored.
// RUN: --unwind 2 --multi-property
// EXPECT: ^  FAILED       \[f\.assertion\.1\] .* recursion unwinding assertion$
// EXPECT: ^  SUCCESS      \[main\.assertion\.1\]
// EXPECT: ^  FAILED       \[main\.assertion\.2\]
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);

int x;
void f(int d) {
  x = d;
  if (d > 0)
    f(d - 1);
}
int main(void) {
  int n = __VERIFIER_nondet_int();
  if (n < 1 || n > 2)
    return 0;
  f(n);
  __VERIFIER_assert(x == n - 1);
  __VERIFIER_assert(x == 0);
  return 0;
}
