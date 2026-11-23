extern float __VERIFIER_nondet_float(void);
extern double __VERIFIER_nondet_double(void);
extern long double __VERIFIER_nondet_long_double(void);
int main(void)
{
  float x = __VERIFIER_nondet_float();
  double y = __VERIFIER_nondet_double();
  long double z = __VERIFIER_nondet_long_double();
  __VERIFIER_assert(!(x == 255.95f && y == 1.0 / 0.0 && z == 3.25L)); // target
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-violationwitness/nondet_return_float_follow
// Needs: --validate-violation-witness --witness witness.yml
// RUN: --64 --validate-violation-witness --witness witness.yml
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
