extern int f(void);

// The value returned by f() is stored in x and never read. The call still
// executes (its side effects are preserved); only the unused assignment of the
// return value is a dead store (CWE-563). Pins call-return site handling.
int main(void)
{
  int x = f();
  return 0;
}
// Imported from ESBMC regression/esbmc/cwe_dead_store_call
// Needs: --dead-store-check
// RUN: --dead-store-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: dead store: assignment to x never read
// EXPECT: ^  CWE: CWE-563$
// Vereda: verdict none
