// #9650: the generated argv set-up must not leave line-0 waypoints.
extern int __VERIFIER_nondet_int(void);

int main(int argc, char *argv[])
{
  int x = __VERIFIER_nondet_int();
  if (x == 3)
    __VERIFIER_assert(0); // reached
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9650_argv_witness_fail
// Needs: --witness-output-yaml -
// RUN: --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: type: "target"
// EXPECT: (?s)\A(?!.*\n *line: 0$)
// Vereda: verdict none
