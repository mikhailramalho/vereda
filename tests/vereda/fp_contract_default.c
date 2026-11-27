// Without a property file too, vereda.py turns contraction off, so a * b + c
// rounds the product and the sum apart, as x86-64 without FMA runs it: the
// product of a = b = 1 + 2^-27 loses its 2^-54 and cancels c = -(1 + 2^-26)
// exactly. An explicit fma() still fuses and keeps the 2^-54.
// EXPECT: ^VERIFICATION SUCCESSFUL$
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
