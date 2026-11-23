/* A program may reuse a name-matched spelling; its own definition wins over the
   builtin lowering (#6904). */
double fabs(double x)
{
  return 42.0;
}

double __VERIFIER_nondet_double(void);

int main(void)
{
  double d = __VERIFIER_nondet_double();
  __VERIFIER_assume(d == -3.0);
  __VERIFIER_assert(fabs(d) == 42.0); // the program's own fabs is called
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_builtin_shadowed
// Needs: --clang-c-irep2-adjust-only
// RUN: --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
