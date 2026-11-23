/* #10080: the only double in the branch is 0x3FB999999999999B. */
extern void reach_error(void);
extern double __VERIFIER_nondet_double(void);
int main()
{
  double x = __VERIFIER_nondet_double();
  if (x > 0.1 && x < 0.10000000000000003)
    reach_error();
  return 0;
}
// Imported from ESBMC regression/witnesses/test_case_generation/github_10080_double
// Needs: --generate-testcase CHECK_FILE testcase.xml contains <testcase coversError="true">\n<input>0\.10000000000000002</input>\n</testcase>
// RUN: --generate-testcase --no-pointer-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
