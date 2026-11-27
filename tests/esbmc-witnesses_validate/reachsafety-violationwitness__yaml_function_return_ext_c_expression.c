extern int __VERIFIER_nondet_int(void);

int main(void)
{
  int x = __VERIFIER_nondet_int();
  int y = x + 1;
  return y;
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-violationwitness/yaml_function_return_ext_c_expression
// Needs: --sv-comp --witness-output-yaml
// RUN: --sv-comp --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --no-assertions --witness-output-yaml - --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^ +format: "ext_c_expression"$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
