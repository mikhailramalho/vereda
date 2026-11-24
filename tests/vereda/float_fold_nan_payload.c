// An operation on constants whose result is a NaN does not fold: with
// --fp2bv camada's encoding gives it the bits hardware gives, the operand's
// payload quieted, and negation flips its sign bit. Under the solver's
// theory the same bits are any NaN's (float_fold_nan_payload_native.c).
// RUN: --fp2bv --unwind 2
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);
union pun { float f; unsigned u; };
union pun snan = {.u = 0x7f800001u}; // a signalling NaN, payload 1
float one = 1.0f;
int main(void) {
  union pun y, z;
  y.f = snan.f * one;
  z.f = -snan.f;
  __VERIFIER_assert(y.f != y.f && y.u == 0x7fc00001u);
  __VERIFIER_assert(z.u == 0xff800001u);
  return 0;
}
