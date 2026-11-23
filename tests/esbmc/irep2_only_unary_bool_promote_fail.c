/* The promoted operand carries the boolean's value, so ~1 is -2 and claiming
   -1 must be caught. */
int b;

int main(void)
{
  __VERIFIER_assert(~(1 || b) == -1); // ~1 is -2, not -1
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_unary_bool_promote_fail
// Needs: --clang-c-irep2-adjust-only
// RUN: --clang-c-irep2-adjust-only --unwind 2
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
