// Under an SV-COMP property file a * b + c rounds the product and the sum
// apart (-ffp-contract=off): with a = b = 1 + 2^-27 and c = -(1 + 2^-26) the
// rounded product cancels c exactly, while a fused one leaves 2^-54.
// RUN: --property-file %S/properties/unreach-call.prp
// EXPECT: ^TRUE$
extern void reach_error(void);
extern double __VERIFIER_nondet_double(void);
extern void __VERIFIER_assume(int);
int main(void) {
  double a = __VERIFIER_nondet_double(), b = __VERIFIER_nondet_double();
  double c = __VERIFIER_nondet_double();
  __VERIFIER_assume(a == 0x1.0000002p0 && b == 0x1.0000002p0 && c == -0x1.0000004p0);
  if (a * b + c != 0)
    reach_error();
  return 0;
}
