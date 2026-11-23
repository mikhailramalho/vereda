// A final dead store: x is assigned and never read before it goes out of
// scope. This is the textbook CWE-563 shape and exercises the end-of-scope
// DEAD handling (no later read keeps the value live).
int main(void)
{
  int x = 6;
  return 0;
}
// Imported from ESBMC regression/esbmc/cwe_dead_store_final
// Needs: --dead-store-check
// RUN: --dead-store-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: dead store: assignment to x never read
// EXPECT: ^  CWE: CWE-563$
// Vereda: verdict none
