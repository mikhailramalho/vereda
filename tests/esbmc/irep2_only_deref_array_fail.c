// Anti-vacuity twin: `*a` writes a[0], not a[1], so the equality is refuted.
int main(void)
{
  int a[3] = {0, 0, 0};

  *a = 7;

  __VERIFIER_assert(a[1] == 7); // *a assigns the second element
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_deref_array_fail
// Needs: --clang-c-irep2-adjust-only
// RUN: --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
