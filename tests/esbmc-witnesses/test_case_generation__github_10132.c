// #10132: an undefined extern is havocked, but no harness feeds it.
extern void reach_error(void);
extern int __VERIFIER_nondet_int(void);
extern int sys_nerr;

int main(void)
{
  int x = __VERIFIER_nondet_int();
  if (x == 42)
    reach_error();
  return 0;
}
// Imported from ESBMC regression/witnesses/test_case_generation/github_10132
// Needs: --generate-testcase CHECK_FILE testcase.xml contains <testcase coversError="true">\n<input>42</input>\n</testcase>
// RUN: --generate-testcase --ub-pointer-check=false --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
