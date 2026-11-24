// Without a property file a * b + c follows clang's default,
// -ffp-contract=on: it may fuse, and Vereda fuses it, so the product of
// a = b = 1 + 2^-27 keeps its 2^-54 and does not cancel c = -(1 + 2^-26).
// An explicit fma() fuses either way.
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);
extern double __VERIFIER_nondet_double(void);
extern void __VERIFIER_assume(int);
extern double fma(double, double, double);
int main(void) {
  double a = __VERIFIER_nondet_double(), b = __VERIFIER_nondet_double();
  double c = __VERIFIER_nondet_double();
  __VERIFIER_assume(a == 0x1.0000002p0 && b == 0x1.0000002p0 && c == -0x1.0000004p0);
  __VERIFIER_assert(fma(a, b, c) == 0x1p-54);
  __VERIFIER_assert(a * b + c == 0);
  return 0;
}
