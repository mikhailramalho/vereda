double __VERIFIER_nondet_double(void);
double sqrt(double);

int main(void)
{
  double d = __VERIFIER_nondet_double();
  __VERIFIER_assume(d == -3.0);

  __VERIFIER_assert(__builtin_fabs(d) == 3.0); // fabs negates a negative
  __VERIFIER_assert(sqrt(d * d) == 3.0); // sqrt of 9 is 3
  __VERIFIER_assert(__builtin_isinf(__builtin_inf())); // inf is infinite
  __VERIFIER_assert(__builtin_huge_val() > d); // huge_val exceeds any finite value
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_builtin_inf_abs
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
