double __VERIFIER_nondet_double(void);

int main(void)
{
  double a = __VERIFIER_nondet_double(), b = __VERIFIER_nondet_double();
  __VERIFIER_assume(a == 2.0 && b == 1.0);

  __VERIFIER_assert(__builtin_isgreater(a, b)); // 2 > 1
  __VERIFIER_assert(__builtin_isgreaterequal(a, b)); // 2 >= 1
  __VERIFIER_assert(!__builtin_isless(a, b)); // 2 is not < 1
  __VERIFIER_assert(__builtin_islessequal(b, b)); // 1 <= 1
  __VERIFIER_assert(__builtin_islessgreater(a, b)); // 2 and 1 differ
  __VERIFIER_assert(!__builtin_isunordered(a, b)); // neither is NaN
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_builtin_ordered
// Needs: --clang-c-irep2-adjust-only
// RUN: --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
