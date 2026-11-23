int main(void)
{
  int x = 0;
  while (x < 2)
  {
    if (x == 0)
      x = 1;
    else
      x = 2;
  }
  return 0;
}
// Imported from ESBMC regression/witnesses_validate/termination-violationwitness/alternating_cycle_terminates
// Needs: --termination --max-inductive-step 3 --validate-violation-witness --witness witness.yml
// RUN: --termination --max-inductive-step 3 --k-step 2 --unlimited-k-steps --no-pointer-check --no-bounds-check --validate-violation-witness --witness witness.yml
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
