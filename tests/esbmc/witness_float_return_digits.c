// A float needs 9 significant digits and a double 17 to read back (#9979).
extern float __VERIFIER_nondet_float(void);
extern double __VERIFIER_nondet_double(void);
int main(void)
{
  float x = __VERIFIER_nondet_float();
  double y = __VERIFIER_nondet_double();
  __VERIFIER_assert(!(x == 0x1.400016p+3f && y == 0x1.0000000000001p+0)); // target
}
// Imported from ESBMC regression/esbmc/witness_float_return_digits
// Needs: --witness-output-yaml w.yml
// RUN: -m64 --witness-output-yaml w.yml --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
