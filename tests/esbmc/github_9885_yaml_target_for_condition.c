extern int __VERIFIER_nondet_int(void);

int main(void)
{
  int x = __VERIFIER_nondet_int();
  for (int i = 0; i < x + 1; i++)
    break;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9885_yaml_target_for_condition
// Needs: --witness-output-yaml -
// RUN: --overflow-check --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: type: "target"\n.*\n.*\n.*\n\s*line: 6\n\s*column: 19$
