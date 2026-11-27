int main(void)
{
  int x = 5; // dead store: overwritten below before any read
  x = 6;
  return 1 / (x - 6); // division by zero: x - 6 == 0
}
// Imported from ESBMC regression/esbmc/cwe_dead_store_fail
// Needs: --dead-store-check --sarif-output
// RUN: --dead-store-check --sarif-output - --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: dead store: assignment to x never read
// EXPECT: "level": "note"
// EXPECT: "ruleId": "dead-store"
// EXPECT: "level": "error"
// EXPECT: "id": "563"
// Vereda: verdict none
