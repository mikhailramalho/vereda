// #9650: a safe main(argc, argv) still verifies.
extern int __VERIFIER_nondet_int(void);

int main(int argc, char *argv[])
{
  int x = __VERIFIER_nondet_int();
  if (x == 3)
    x = 4;
  return x == 3;
}
// Imported from ESBMC regression/esbmc/github_9650_argv_witness
// Needs: --witness-output-yaml
// RUN: --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
