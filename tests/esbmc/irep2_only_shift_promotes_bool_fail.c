// Anti-vacuity twin: promoted, the shift yields 2, so 1 is refuted -- and an
// unpromoted one-bit result would not be 1 either, it would abort.
int main(void)
{
  int i = 1, j = 5, nc_B = 3;

  int found = (j > nc_B - 1) << i;

  __VERIFIER_assert(found == 1); // the shift result is one
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_shift_promotes_bool_fail
// Needs: --clang-c-irep2-adjust-only
// RUN: --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
