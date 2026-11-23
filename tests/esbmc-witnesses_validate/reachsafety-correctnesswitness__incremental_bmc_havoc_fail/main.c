extern void reach_error(void);
int main()
{
  unsigned i = 0;
  while (1)
  {
    i++;
    if (i == 5)
      reach_error();
  }
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-correctnesswitness/incremental_bmc_havoc_fail
// Needs: --validate-correctness-witness --witness witness.yml
// RUN: --incremental-bmc --validate-correctness-witness --witness witness.yml
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
