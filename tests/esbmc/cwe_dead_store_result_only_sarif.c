// With --result-only the success/error trace reporters bail out early, so the
// dead-store advisory must reach SARIF from a verdict-independent point.
// Pins that the CWE-563 note is emitted here even though no trace is built.
int main(void)
{
  int x = 5;
  x = 6;
  return x;
}
// Imported from ESBMC regression/esbmc/cwe_dead_store_result_only_sarif
// Needs: --dead-store-check --result-only --sarif-output -
// RUN: --dead-store-check --result-only --sarif-output -
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: "level": "note"
// EXPECT: "id": "563"
// Vereda: verdict none
