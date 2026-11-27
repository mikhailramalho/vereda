int main()
{
  // gcc eats this
  int i;
  extern int i;
}
// Imported from ESBMC regression/cbmc/01_cbmc_ExternExtern2
// RUN: --no-incremental-bmc
// EXPECT: ^ERROR: PARSING ERROR$
// Vereda: unmet EXPECT: ^ERROR: PARSING ERROR$
// MODE: FUTURE
// Vereda: verdict none
