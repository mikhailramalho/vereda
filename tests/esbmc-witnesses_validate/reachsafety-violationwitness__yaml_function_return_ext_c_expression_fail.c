extern int __VERIFIER_nondet_int(void);

int main(void)
{
  int x = __VERIFIER_nondet_int();
  if (x < 100)
    x = x + 1;
  return x;
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-violationwitness/yaml_function_return_ext_c_expression_fail
// Needs: --sv-comp --no-assertions --witness-output-yaml -
// RUN: --sv-comp --overflow-check --no-assertions --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
