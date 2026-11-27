#include <assert.h>

int __VERIFIER_nondet_int(void);

int main()
{
  int x = __VERIFIER_nondet_int();
  assert(x != 42);
  return 0;
}
// Imported from ESBMC regression/witnesses/test_case_generation/ctest_gen_output_dir
// Needs: --generate-ctest-testcase --ctest-output-dir custom-out/nested/dir CHECK_FILE custom-out/nested/dir/CMakeLists.txt contains project\(ESBMCGeneratedTest C\) CHECK_FILE custom-out/nested/dir/test_case.c contains __VERIFIER_nondet_int CHECK_FILE custom-out/nested/dir/esbmc_verifier.h contains __VERIFIER_assume
// RUN: --generate-ctest-testcase --ctest-output-dir custom-out/nested/dir --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^Generated CTest test case\: test_case\.c$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
