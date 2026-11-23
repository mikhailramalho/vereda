/* C11 6.5.3.3: the operand of unary - and ~ undergoes integer promotion, so a
   boolean one -- a comparison, || or && -- becomes int. The IREP2 pass handled
   only the complex unary case, so the boolean operand reached the solver where
   a bitvector was wanted and bitwuzla aborted (#4078). */
int a, b;

int main(void)
{
  __VERIFIER_assert(~(a || b) == -1); // ~0 is -1
  __VERIFIER_assert(-(a || b) == 0); // -0 is 0
  __VERIFIER_assert(~(1 || b) == -2); // ~1 is -2
  __VERIFIER_assert(-(1 || b) == -1); // -1 is -1
  __VERIFIER_assert(~(a && b) == -1); // && promotes too
  __VERIFIER_assert(~(a < 1) == -2); // a comparison promotes too
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_unary_bool_promote
// Needs: --clang-c-irep2-adjust-only
// RUN: --clang-c-irep2-adjust-only --unwind 2
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
