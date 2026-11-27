/* #10080: a binary128 long double needs 36 digits to read back as itself. */
extern void reach_error(void);
extern long double __VERIFIER_nondet_long_double(void);
int main()
{
  long double x = __VERIFIER_nondet_long_double();
  long double lo = 0.1L;
  if (x > lo && x < lo + 0x1p-115L)
    reach_error();
  return 0;
}
// Imported from ESBMC regression/witnesses/test_case_generation/github_10080_long_double
// Needs: --generate-testcase --i386-linux
// RUN: --generate-testcase --ub-pointer-check=false -m64 --i386-linux
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
