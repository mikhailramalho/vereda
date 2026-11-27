int __VERIFIER_nondet_int();
int g;

int f(int a, int b)
{
  if (a)
    return 1;
  if (b)
    g++;
  return 0;
}

int main()
{
  f(__VERIFIER_nondet_int(), __VERIFIER_nondet_int());
  f(__VERIFIER_nondet_int(), __VERIFIER_nondet_int());
  __VERIFIER_assert(g == 0); // g
  return 0;
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-violationwitness/branching_after_dead_return
// Needs: --validate-violation-witness --witness witness.yml
// RUN: --validate-violation-witness --witness witness.yml --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
