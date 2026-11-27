int main(void)
{
  int x = 5; // dead store: overwritten below before any read
  x = 6;
  return x;
}
// Imported from ESBMC regression/esbmc/cwe_dead_store
// Needs: --dead-store-check
// RUN: --dead-store-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: dead store: assignment to x never read
// EXPECT: ^  CWE: CWE-563$
// Vereda: verdict none
