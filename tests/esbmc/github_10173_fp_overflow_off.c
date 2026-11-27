extern double __VERIFIER_nondet_double(void);
int main()
{
  double a = __VERIFIER_nondet_double();
  double b = a * 10.0;
  return (int)(b != b);
}
// Imported from ESBMC regression/esbmc/github_10173_fp_overflow_off
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-pointer-check=false --no-assertions --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
