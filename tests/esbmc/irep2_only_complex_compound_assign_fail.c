/* The lowering carries the real arithmetic, so a wrong expected component must
   be caught rather than excused by an undecomposed node. */
int main(void)
{
  _Complex float a = 3.0f + 4.0fi;

  a *= 2.0f;
  __VERIFIER_assert(__imag__ a == 4.0f); // *= imag is 8, not 4

  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_complex_compound_assign_fail
// Needs: --clang-c-irep2-adjust-only
// RUN: --clang-c-irep2-adjust-only --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
