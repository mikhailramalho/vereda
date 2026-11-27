extern int __VERIFIER_nondet_int(void);

int main(void)
{
  int x = __VERIFIER_nondet_int();
  for (int i = x + 1; i < 3; i++)
    ;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9885_yaml_target_initialiser
// Needs: --witness-output-yaml
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --unwind 4 --no-unwinding-assertions --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: type: "target"\n.*\n.*\n.*\n\s*line: 6\n\s*column: 16$
// Vereda: verdict none
