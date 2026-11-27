// #10132: the error depends on an undefined extern's value, which is still
// not a test input; only the two __VERIFIER_nondet_int() calls are.
extern void reach_error(void);
extern int __VERIFIER_nondet_int(void);
extern int sys_nerr;

int main(void)
{
  int x = __VERIFIER_nondet_int();
  int y = __VERIFIER_nondet_int();
  if (x == 42 && sys_nerr == 3 && y == 7)
    reach_error();
  return 0;
}
// Imported from ESBMC regression/witnesses/test_case_generation/github_10132_read
// Needs: --generate-testcase CHECK_FILE testcase.xml contains <testcase coversError="true">\n<input>42</input>\n<input>7</input>\n</testcase>
// RUN: --generate-testcase --ub-pointer-check=false --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
