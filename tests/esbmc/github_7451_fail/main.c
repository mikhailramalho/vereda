int combine(int a, int b)
{
  return a - b;
}
// Imported from ESBMC regression/esbmc/github_7451_fail
// RUN: --no-incremental-bmc
// FILES: weak.c
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: line 10 +assertion
// Vereda: unmet EXPECT: line 10 +assertion
// MODE: FUTURE
// Vereda: verdict FAILED
