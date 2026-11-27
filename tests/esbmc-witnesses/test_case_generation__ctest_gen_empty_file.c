#include <assert.h>

int __VERIFIER_nondet_int(void);

int main()
{
  int x = __VERIFIER_nondet_int();
  assert(x != 42);
  return 0;
}
// Imported from ESBMC regression/witnesses/test_case_generation/ctest_gen_empty_file
// Needs: --generate-ctest-testcase --ctest-output-dir out SEED_FILE out/CMakeLists.txt CHECK_FILE out/CMakeLists.txt contains project\(ESBMCGeneratedTest C\)
// RUN: --generate-ctest-testcase --ctest-output-dir out --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^Generated CTest test case\: test_case\.c$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
