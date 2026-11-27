extern int __VERIFIER_nondet_int(void);

int next(int x)
{
  return x + 1;
}

int main(void)
{
  return next(__VERIFIER_nondet_int());
}
// Imported from ESBMC regression/esbmc/github_9885_yaml_target_return
// Needs: --witness-output-yaml
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --witness-output-yaml - --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: type: "target"\n.*\n.*\n.*\n\s*line: 5\n\s*column: 10$
// Vereda: verdict none
