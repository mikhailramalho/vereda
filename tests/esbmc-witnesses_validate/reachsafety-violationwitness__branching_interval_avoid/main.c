extern void reach_error(void);
void check(int cond)
{
  if (!cond)
  {
    reach_error();
  }
}
int main(void)
{
  check(0);
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-violationwitness/branching_interval_avoid
// Needs: --sv-comp --interval-analysis --validate-violation-witness --witness witness.yml
// RUN: -m32 --sv-comp --std c11 --interval-analysis --validate-violation-witness --witness witness.yml --unwind 4
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
