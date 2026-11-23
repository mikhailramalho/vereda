// RUN: --property-file %S/properties/no-overflow.prp
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: ^TRUE$
// Guarded signed arithmetic does not overflow, and unsigned arithmetic
// wraps by definition.
extern int __VERIFIER_nondet_int(void);
extern unsigned __VERIFIER_nondet_uint(void);

int main(void) {
  int x = __VERIFIER_nondet_int();
  unsigned u = __VERIFIER_nondet_uint();
  if (x < 2147483647)
    x = x + 1;
  return x - (int)(u * 2u > 0);
}
