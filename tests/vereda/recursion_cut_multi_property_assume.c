// With --no-unwinding-assertions the bound is assumed, so even under
// --multi-property the path ends at the cut call.
// RUN: --unwind 2 --multi-property --no-unwinding-assertions
// EXPECT: ^VERIFICATION SUCCESSFUL$
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
