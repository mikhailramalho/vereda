// Loop body with one branch, adapted from regression/goto-coverage/k_path_cov_2/.
// Uses __VERIFIER_nondet_int() so the generated CTest cases compile.
extern int __VERIFIER_nondet_int(void);

int main(void)
{
  int x = __VERIFIER_nondet_int();
  for (int i = 0; i < 3; i++)
  {
    if (x > 0)
      x = x - 1;
    else
      x = x + 1;
  }
  return x;
}
// Imported from ESBMC regression/witnesses/test_case_generation/ctest_gen_kpath_1
// Needs: --k-path-coverage=2 --generate-ctest-testcase
// RUN: --k-path-coverage=2 --unwind 4 --no-unwinding-assertions --generate-ctest-testcase
// MODE: FUTURE
// EXPECT: ^Generated 3 CTest test case\(s\) with CMakeLists\.txt
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
