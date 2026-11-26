// No function_return waypoint for a NaN: \result == NAN holds for no value (#9979).
extern float __VERIFIER_nondet_float(void);
int main(void)
{
  float x = __VERIFIER_nondet_float();
  __VERIFIER_assert(x == x); // target
}
// Imported from ESBMC regression/esbmc/witness_float_return_nan
// Needs: --witness-output-yaml w.yml CHECK_FILE w.yml absent function_return
// RUN: -m64 --witness-output-yaml w.yml
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
