// float_fold_nan_payload.c under the solver's theory (the default), where a
// NaN result's bits are any NaN's: the fold must not pick some, so the
// payload check fails.
// RUN: --unwind 2
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  FAILED +\[main\.assertion\.2\]  line 15
extern void __VERIFIER_assert(int);
union pun { float f; unsigned u; };
union pun snan = {.u = 0x7f800001u}; // a signalling NaN, payload 1
float one = 1.0f;
int main(void) {
  union pun y;
  y.f = snan.f * one;
  __VERIFIER_assert(y.f != y.f);
  __VERIFIER_assert(y.u == 0x7fc00001u);
  return 0;
}
