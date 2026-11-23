double __VERIFIER_nondet_double(void);

/* Each value is chosen so that the four predicates disagree on it: an infinity
   separates isinf from isnan and isfinite, and a zero separates isnormal from
   isfinite. A test over 1.0 alone cannot tell any of them apart. */
int main(void)
{
  double d = __VERIFIER_nondet_double(), z = __VERIFIER_nondet_double();
  __VERIFIER_assume(d == 1.0 && z == 0.0);
  const double i = __builtin_inf();

  __VERIFIER_assert(!__builtin_isnan(d)); // 1.0 is not NaN
  __VERIFIER_assert(!__builtin_isnan(i)); // an infinity is not NaN
  __VERIFIER_assert(__builtin_isinf(i)); // an infinity is infinite
  __VERIFIER_assert(!__builtin_isinf(d)); // 1.0 is not infinite
  __VERIFIER_assert(__builtin_isfinite(d)); // 1.0 is finite
  __VERIFIER_assert(!__builtin_isfinite(i)); // an infinity is not finite
  __VERIFIER_assert(__builtin_isnormal(d)); // 1.0 is normal
  __VERIFIER_assert(!__builtin_isnormal(z)); // 0.0 is finite but not normal
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_builtin_float_class
// Needs: --clang-c-irep2-adjust-only
// RUN: --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
