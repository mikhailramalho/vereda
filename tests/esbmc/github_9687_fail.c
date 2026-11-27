extern int __VERIFIER_nondet_int(void);
int main(void)
{
  int x = __VERIFIER_nondet_int();
  int y = x + 1;
  return y;
}
// Imported from ESBMC regression/esbmc/github_9687_fail
// Needs: --witness-output-yaml -
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --no-assertions --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^    format_version: "2\.1"$
// EXPECT: ^              format: "ext_c_expression"$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
