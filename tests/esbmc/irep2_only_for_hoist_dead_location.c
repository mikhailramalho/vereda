/* A non-block for-body leaves the hoisted wrapper block with no end location,
   so the DEAD unwound at its close is unlocated. */
int main(void)
{
  int s = 0;
  for (int i = 0; i < 3; i++)
    s = s + i;
  return s;
}
// Imported from ESBMC regression/esbmc/irep2_only_for_hoist_dead_location
// Needs: --goto-functions-only --clang-c-irep2-adjust-only
// RUN: --goto-functions-only --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: //\s*\d+ no location\n[^\n]*DEAD [^\n]*@F@main@i$
// Vereda: verdict none
