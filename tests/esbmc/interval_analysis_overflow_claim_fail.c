// Interval analysis runs before the overflow checks are added, so folding
// a + 1 into a constant used to drop its overflow claim.
int main()
{
  int a = 2147483647;
  int b = a + 1;
  return b;
}
// Imported from ESBMC regression/esbmc/interval_analysis_overflow_claim_fail
// Needs: --interval-analysis
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --interval-analysis
// MODE: FUTURE
// EXPECT: ^  arithmetic overflow on add$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
