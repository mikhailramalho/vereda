int main(void)
{
  int x = 5; // dead store: overwritten below before any read
  x = 6;
  return x;
}
// Imported from ESBMC regression/esbmc/cwe_dead_store_sarif
// Needs: --dead-store-check --sarif-output
// RUN: --dead-store-check --sarif-output -
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: "level": "note"
// EXPECT: "ruleId": "dead-store"
// EXPECT: "id": "563"
// EXPECT: "version": "4.20"
// Vereda: verdict none
