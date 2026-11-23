double __VERIFIER_nondet_double(void);

int main(void)
{
  double p = __VERIFIER_nondet_double(), n = __VERIFIER_nondet_double();
  __VERIFIER_assume(p == 1.0 && n == -1.0);

  __VERIFIER_assert(!__builtin_signbit(p)); // 1.0 is positive
  __VERIFIER_assert(__builtin_signbit(n)); // -1.0 is negative
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_builtin_signbit
// Needs: --clang-c-irep2-adjust-only
// RUN: --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
