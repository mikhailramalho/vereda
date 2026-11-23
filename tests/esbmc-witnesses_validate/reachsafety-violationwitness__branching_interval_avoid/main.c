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
// Needs: --sv-comp --std c11 --interval-analysis --validate-violation-witness --witness witness.yml
// RUN: --32 --sv-comp --std c11 --interval-analysis --validate-violation-witness --witness witness.yml --unwind 4
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
